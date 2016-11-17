from django.conf.urls import url

from . import views

app_name = "ebna"
urlpatterns = [
    url(r'^$', views.index_view, name='index'),
    #url(r'^people/$', views.people_view, name='people'),
]