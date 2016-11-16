from django.shortcuts import render
from django.http import HttpResponse

from models import *

def index_view(request):
    parties = Party.objects.order_by("name")
    context = {
        "parties": parties,
    }
    return render(request, "mpbio/index.html", context)


def people_view(request):

    parties = Party.objects.order_by("name")
    people = Person.objects.all()

    columns = ["name", "occupation", "party"]

    # check GET queries and filter people list
    filter_string = ""
    if (request.method == "GET"):
        for c in columns:
            try:
                query = request.GET["include_" + c]
                if len(query):
                    people = eval("people.filter(%1__icontains=query)" % c)
                    filter_string += "%2(%s) " % (c, query)
            except:
                pass

    context = {
        "parties": parties,
        "people": people,
        "people_size": len(people),
        "filter_string": filter_string,
        "columns": columns,
    }
    return render(request, "mpbio/people.html", context)


def party_view(request, pk):
    try:
        party = Party.objects.get(pk=pk)
    except Party.DoesNotExist:
        return HttpResponse("Unknown party key %s" % str(pk))

    parties = Party.objects.order_by("name")
    people = Person.objects.filter(party__exact=party).order_by("name")

    filter = ""
    if (request.method == "GET"):
        try:
            inc_name = request.GET["include_name"]
            if len(inc_name):
                people = people.filter(name__icontains=inc_name)
                filter += "Name(%s) " % inc_name
        except:
            pass
        try:
            inc_occ = request.GET["include_occupation"]
            if len(inc_occ):
                people = people.filter(occupation__icontains=inc_occ)
                filter += "Beruf(%s) " % inc_occ
        except:
            pass

    context = {
        "party": party,
        "parties": parties,
        "people": people,
        "filter": filter,
        "people_size": len(people),
        "inc_name": inc_name, "inc_occ": inc_occ,
    }
    return render(request, "mpbio/party.html", context)


def person_view(request, pk):
    try:
        person = Person.objects.get(pk=pk)
    except Person.DoesNotExist:
        return HttpResponse("Unknown person key %s" % str(pk))

    statements = Statement.objects.filter(person__exact=person)
    memberships = Membership.objects.filter(person__exact=person).order_by("rank")

    context = {
        "person": person,
        "statements": statements,
        "memberships": memberships,
    }
    return render(request, "mpbio/person.html", context)

