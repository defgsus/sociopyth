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

def test1():
    print get_party("FDP")
    p = get_person("Konrad Adenauer")
    p.party = get_party("CDU")
    p.district = get_district("Wahlkreis 1")
    p.parliament = get_parliament("Bundestag")
    p.save()


def add_people(people):
    for p in people:
        m = get_person(p.name)
        m.party = get_party(p.party)
        m.district = get_district(p.wahlkreis)
        m.parliament = get_parliament(p.gremium)
        m.occupation = p.occupation
        m.url = p.url
        m.img_url = p.img_url
        m.birth = p.birth
        m.save()
        print("added %s " % unicode(m))


import mpbio_tools
people = mpbio_tools.bio_load_xml("../abgeordnete/xml/bundestag16.xml")
add_people(people)
