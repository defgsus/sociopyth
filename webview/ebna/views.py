from django.shortcuts import render
from django.http import HttpResponse

from models import *

def index_view(request):
    context = { }
    return render(request, "ebna/index.html", context)
