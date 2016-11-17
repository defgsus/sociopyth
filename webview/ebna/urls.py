from django.conf.urls import url

from . import views

app_name = "ebna"
urlpatterns = [
    url(r'^$', views.index_view, name='index'),
    url(r'^plot.png$', views.image_plot, name='plot'),
]