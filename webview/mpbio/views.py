# coding=utf8
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

    columns = ["name", "occupation", "party", "url"]

    # check GET queries and filter people list
    filter_string = ""
    filter_values = []
    if (request.method == "GET"):
        for c in columns:
            try:
                query = request.GET["include_" + c]
                if query:
                    filter_code = "%s__icontains=query" % c
                    # XXX geht alles garnicht!
                    if c == "party":
                        try:
                            party = Party.objects.get(name__icontains=query)
                            filter_code = "pk=party"
                        except Party.DoesNotExist:
                            pass
                    print filter_code
                    people = eval("people.filter(" + filter_code + ")")
                    filter_string += "%s(%s) " % (c, query)
                filter_values.append( query )
            except:
                filter_values.append("")

    context = {
        "parties": parties,
        "people": people,
        "people_size": len(people),
        "filter_string": filter_string,
        "filter_values": filter_values,
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
    inc_name = ""
    inc_occ = ""
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


def wordcloud_render(request, pk):
    from wordcloud import WordCloud

    text = ""
    try:
        person = Person.objects.get(pk=pk)

        statements = Statement.objects.filter(person__exact=person)
        for s in statements:
            text += s.text + " "

        statements = Membership.objects.filter(person__exact=person)
        for s in statements:
            text += s.text + " "
    except:
        pass

    if not text:
        text = "no data"

    stopwords = ["der", "die", "das", "bis", "in", "und", "des", "den", "im", "seit",
                 "an", "für", "am", "bei", "als", "mit", "von", "vom", "Mitglied"]

    width = 320
    height = 200
    if request.method=="GET":
        try:
            width = int(request.GET.get("width"))
            height = int(request.GET.get("height"))
        except:
            pass

    wc = WordCloud(width=width, height=height, stopwords=stopwords).generate(text)

    image = wc.to_image()

    response = HttpResponse(content_type="image/png")
    image.save(response, format="png")
    return response
    #return HttpResponse("Yippie %s" % str(pk))