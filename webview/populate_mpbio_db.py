from django.db import models, transaction
# from django import setup
import django
django.setup()


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


def get_statement(text, person):
    try:
        return Statement.objects.get(text=text, person=person)
    except Statement.DoesNotExist:
        p = Statement(text=text, person=person)
        return p
    except:
        raise


def get_membership(text, rank, person):
    try:
        return Membership.objects.get(text=text, rank=rank, person=person)
    except Membership.DoesNotExist:
        p = Membership(text=text, rank=rank, person=person)
        p.save()
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
    with transaction.atomic():
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

            # print p.statements
            for stmt in p.statements:
                s = get_statement(stmt, m)
                s.person = m
                s.save()

            for k in range(len(p.member)):
                for stmt in p.member[k]:
                    get_membership(stmt, k, m)

            print("added %s " % unicode(m))


import mpbio_tools
people = mpbio_tools.bio_load_xml("../abgeordnete/xml/bundestag16.xml")
add_people(people)
people = mpbio_tools.bio_load_xml("../abgeordnete/xml/bundestag17.xml")
add_people(people)
people = mpbio_tools.bio_load_xml("../abgeordnete/xml/bundestag18.xml")
add_people(people)
