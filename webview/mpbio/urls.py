from django.conf.urls import url

from . import views

app_name = "mpbio"
urlpatterns = [
    url(r'^$', views.index_view, name='index'),
    url(r'^people/$', views.people_view, name='people'),
    url(r'^party/(?P<pk>[0-9]+)/$', views.party_view, name='party'),
    url(r'^person/(?P<pk>[0-9]+)/$', views.person_view, name='person'),
    url(r'^wordcloud_(?P<pk>[0-9]+).png$', views.wordcloud_render, name='wordcloud'),
]