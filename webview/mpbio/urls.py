from django.conf.urls import url

from . import views

app_name = "mpbio"
urlpatterns = [
    url(r'^$', views.index_view, name='index'),
    url(r'^party/(?P<pk>[0-9]+)/$', views.party_view, name='party'),
    url(r'^person/(?P<pk>[0-9]+)/$', views.person_view, name='person'),
]