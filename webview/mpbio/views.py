from django.shortcuts import render
from django.http import HttpResponse

from models import *

def index_view(request):
    parties = Party.objects.order_by("name")
    context = {
        "parties": parties,
    }
    return render(request, "mpbio/index.html", context)

def party_view(request, pk):
    party = Party.objects.get(pk=pk)
    if party is None:
        return HttpResponse("Unknown party key %s" % str(pk))

    parties = Party.objects.order_by("name")
    people = Person.objects.filter(party__exact=party).order_by("name")
    context = {
        "party": party,
        "parties": parties,
        "people": people
    }
    return render(request, "mpbio/party.html", context)

def person_view(request, pk):
    person = Person.objects.get(pk=pk)
    if person is None:
        return HttpResponse("Unknown person key %s" % str(pk))
    context = {
        "person": person
    }
    return render(request, "mpbio/person.html", context)

