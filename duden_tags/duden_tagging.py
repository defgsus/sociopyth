# coding=utf-8

# little prog used to tag a list of words with the type of word
# using duden.de


import requests
import csv
from bs4 import BeautifulSoup

# return a list of urls for the given word in urls set
def search_word(word, urls):
	
	base_url = "http://www.duden.de"
	search_url = base_url + "/suchen/dudenonline/" + word

	try:
		r = requests.get(search_url)
	except ConnectionError:
		print "connection failed " + search_url

	#soup = BeautifulSoup('blabla <a href="/rechtschreibung/sie">sie</a><a href="/rechtschreibung/sie_auch">sie auch</a><a href="/rechtschreibung/sie">sie</a>')
	soup = BeautifulSoup(r.content)
	
	for link in soup.find_all("a"):
		url = link.get("href")
		if type(url) is str:
			if url.find("/rechtschreibung/") >= 0 and url.find(word) > 0:
				if link.find("strong") >= 0:
					urls.add( base_url + url )
	
	return urls

"""	
urls = set()
	
search_word("nicht", urls)
search_word("doch", urls)

print urls
"""


	
inputf = open("z_word_freq2012.csv", "rb")
outputf = open("word_tags.csv", "w")

"""
def get_word_tag(word):

	base_url = "http://www.duden.de/rechtschreibung/"

	word = word.replace("ä", "ae")
	word = word.replace("ö", "oe")
	word = word.replace("ü", "ue")
	word = word.replace("ß", "sz")
	
	try:
		r = requests.get(base_url + word)
	except ConnectionError:
		print "connection failed " + base_url + word

	# find wortart 
	tag = '<span class="wortart">'
	#rtext = 'hallo abc	<span class="wortart">das, die</span>'
	i = r.text.find(tag)
	if i < 0 : 
		print "UNKNOWN " + word
		return "False"
	i += len(tag)  
	text = r.text[i:]
	i = text.find('</span>');
	text = text[0:i]

	wortart = text.split(",")
	
	if len(wortart) < 1: return "False"
	
	ret = wortart[0]
	if (len(wortart) > 1): ret += "," + wortart[1] 
	else: ret += ", null"

	return ret
"""


def get_word_tag_url(url):

	try:
		r = requests.get(url)
	except requests.exceptions.ConnectionError:
		print "connection failed " + url
		return "False"
		
	# find wortart 
	tag = '<span class="wortart">'
	#rtext = 'hallo abc	<span class="wortart">das, die</span>'
	i = r.content.find(tag)
	if i < 0 : 
		print "UNKNOWN " + url
		return "False"
	i += len(tag)  
	text = r.content[i:]
	i = text.find('</span>');
	text = text[0:i]

	wortart = text.split(",")
	
	if len(wortart) < 1: return "False"
	
	ret = wortart[0]
	if (len(wortart) > 1): ret += "," + wortart[1] 
	else: ret += ", null"

	return ret

	
def tag_all_words():

	# read csv words
	icsv = csv.reader(inputf)
	words = list(icsv)

	# tag each
	for i in words:
		
		# find links

		urls = set()
	
		search_word(i[0], urls)
		
		for j in urls:
			wordline = get_word_tag_url(j)
			if wordline != "False":
				wordline = i[0] + ", " + i[1] + ", " + wordline
				outputf.write((wordline + "\n").encode('UTF-8'))
				print wordline

#get_word_tag_url("http://balbal.bla")
tag_all_words()
#print get_word_tag("der")
#print get_word_tag("Straße")

inputf.close()
outputf.close()



