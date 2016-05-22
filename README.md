# sociopyth

Collection of python scripts to download social relevant stuff from the web. 
Most scripts are meant to be run from a server that has a good connection. 
This repo is used to easily update the source on the server. 

There will also be applications for parsing and organizing data, 
also in other, non-python languages.



## interesting places to scan

### http://www.kek-online.de
"Kommission zur Ermittlung der Konzentration im Medienbereich"
A constitutional agency that tracks ownership and share relations of all german media titles and companies (papers, tv, radio, online)
see /kek_medien 

### http://bundestag.de
all german parliament publications

see /bundestag_down for scripts to download all publications as .pdf (sadly) since the founding of the Bundestag.

see /abgeordnete for collecting the public biographies of all members of parliament (Bundestag & Landtag). Biographies contain educational info and memberships in public and private organizations. The scripts try to collect and separate this info in meaningful classes. Annoyingly, they need to be adjusted for the website of every legislation and for each individual Landtag.

see /bioviewer/bioviewer_qt for a Qt-based display tool for the biography data with full-text search :)

### http://wikidata.org
machine readable database of everything

### http://nndb.org
"Noteworthy names database", relational network database of people 
Nicely compiled by Soylent Communications

### https://viaf.org
"Virtual International Authority File", an index of authors/publishers and national archives and authority files
- selected co-authors
- json output
- xml-dump (7.5GB): http://viaf.org/viaf/data


## other interesting stuff
 
http://opendatacommons.org
"A set of legal tools to help you provide and use Open Data"
