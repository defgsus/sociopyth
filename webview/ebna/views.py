from django.shortcuts import render
from django.http import HttpResponse

from models import *

def get_get(request, name, default):
    try:
        return request.GET[name]
    except:
        return default

def filter_freqs(freqs, values, q_items):
    """Apply dict in values to filter the QuerySet freqs"""
    f = values.get("year_min", 1000)
    q_items[4] = f
    freqs = freqs.filter(year__gte = f)

    f = values.get("year_max", 3000)
    q_items[5] = f
    freqs = freqs.filter(year__lte=f)

    f = values.get("search_term", "")
    q_items[0] = f
    if f:
        freqs = freqs.filter(search_term__name__icontains = f)

    f = values.get("category", "nouns")
    q_items[1] = f
    if f:
        freqs = freqs.filter(category__name__icontains = f)

    f = values.get("source", "")
    q_items[2] = f
    if f:
        freqs = freqs.filter(source__name__icontains = f)

    f = values.get("phrase", "")
    q_items[3] = f
    if f:
        freqs = freqs.filter(phrase__text__icontains = f)


    f = values.get("sort", "-count")
    q_items[8] = f
    if f:
        freqs = freqs.order_by(f)

    return freqs

def index_view(request):

    q_items = range(9)
    freqs = Frequency.objects.all()
    freqs = filter_freqs(freqs, request.GET, q_items)

    context = {
        "num_hits": len(freqs),
        "freqs": freqs[:300],
        "q_items": q_items,
        "categories": Category.objects.all(),
        "sort_terms": ["phrase", "year", "-count", "-f1", "-f2"],
    }
    return render(request, "ebna/index.html", context)
