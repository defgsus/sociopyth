from django.db import models
#from django import setup
import django
django.setup()

#import mpbio

def get_party(name):
    try:
        return Party.objects.get(name=name)
    except Party.DoesNotExist:
        p = Party(name=name)
        p.save()
        return p
    except:
        raise

def get_district(name):
    try:
        return District.objects.get(name=name)
    except District.DoesNotExist:
        p = District(name=name)
        p.save()
        return p
    except:
        raise

def get_parliament(name):
    try:
        return Parliament.objects.get(name=name)
    except Parliament.DoesNotExist:
        p = Parliament(name=name)
        p.save()
        return p
    except:
        raise

def get_person(name):
    try:
        return Person.objects.get(name=name)
    except Person.DoesNotExist:
        p = Person(name=name)
        return p
    except:
        raise


from mpbio.models import *

print get_party("FDP")
p = get_person("Konrad Adenauer")
p.party = get_party("CDU")
p.district = get_district("Wahlkreis 1")
p.parliament = get_parliament("Bundestag")
p.save()
