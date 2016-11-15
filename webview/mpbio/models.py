from __future__ import unicode_literals
from django.db import models
from datetime import datetime


class Party(models.Model):
    name = models.TextField(unique=True)

    def __str__(self): return "Party(%s)" % self.name


class District(models.Model):
    name = models.TextField(unique=True)

    def __str__(self): return "District(%s)" % self.name


class Parliament(models.Model):
    name = models.TextField(unique=True)

    def __str__(self): return "Parliament(%s)" % self.name


class Person(models.Model):
    name = models.TextField(unique=True)
    url = models.URLField()
    img_url = models.URLField()
    occupation = models.TextField()
    birth = models.TextField()
    party = models.ForeignKey(Party, on_delete=models.CASCADE)
    district = models.ForeignKey(District, on_delete=models.CASCADE)
    # period = ""
    parliament = models.ForeignKey(Parliament, on_delete=models.CASCADE)
    # statements = models.
    # member = []  # list of list of str

    def __str__(self): return "Person(%s)" % self.name


class Statement(models.Model):
    text = models.TextField()
    person = models.ForeignKey(Person, on_delete=models.CASCADE)

    def __str__(self): return "Statement(%s)" % self.person.name


class Membership(models.Model):
    text = models.TextField()
    rank = models.IntegerField()
    person = models.ForeignKey(Person, on_delete=models.CASCADE)

    def __str__(self): return "Membership(%s)" % self.person.name



