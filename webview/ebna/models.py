from __future__ import unicode_literals
from django.db import models


class Source(models.Model):
    name = models.CharField(max_length=256)

    def __str__(self): return "Source(%s)" % self.text


class SearchTerm(models.Model):
    name = models.CharField(max_length=512)

    def __str__(self): return "SearchTerm(%s)" % self.name


class Category(models.Model):
    name = models.CharField(max_length=256)

    def __str__(self): return "Category(%s)" % self.name


class Phrase(models.Model):
    text = models.CharField(max_length=256)

    def __str__(self): return "Phrase(%s)" % self.text


class Frequency(models.Model):
    count = models.BigIntegerField()
    f1 = models.FloatField()
    f2 = models.FloatField()

    year = models.IntegerField()
    phrase = models.ForeignKey(Phrase, on_delete=models.CASCADE)
    source = models.ForeignKey(Source, on_delete=models.CASCADE)
    category = models.ForeignKey(Category, on_delete=models.CASCADE)
    search_term = models.ForeignKey(SearchTerm, on_delete=models.CASCADE)

    def __str__(self): return "Frequency(%s, %s, %d)" % (self.phrase.text, self.source.name, self.year)
