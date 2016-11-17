#!/usr/bin/env python
# coding=UTF-8


from os import path
import random
import codecs

"""from wordcloud import WordCloud
import matplotlib.pyplot as plt

def my_color_func(word, font_size, position, orientation, random_state=None, **kwargs):
   return "hsl(%d, 100%%, %d%%)" % (position[0]/8, random.randint(60, 100))


def show_cloud(d):
	# Generate a word cloud image
	wordcloud = WordCloud(width=800, height=500, max_font_size=1000, max_words=10000, color_func=my_color_func).generate_from_frequencies(d)
	plt.imshow(wordcloud)
	plt.axis("off")
	plt.show()
"""


def parse_enke_section(lines, start_index):
    """
    Returns a dict with the info from the enke-section
    :param lines: a list of lines representing the enke-file
    :param start_index: index of first line in section
    :return: a dict, key = phrase, value = list of frequencies (length 3)
    """
    d = { }

    for i in range(start_index, len(lines)):
        tok = lines[i].split(' ')
        # print tok
        # start marker for each year
        if (len(tok) >= 3 and tok[0] == 'year' and tok[2] == 'has'):
            break;
        # section start
        if (len(tok) >= 4 and tok[1] == '-' and tok[3] == '-'):
            break;
        # word table entry
        if '::' in tok:
            for entry in lines[i].split(':: '):
                if (len(entry) == 0): continue
                try:
                    pair = entry.split(' - ')
                    if (len(pair) < 2):
                        print "error in " + str(pair)
                        break;
                    freqs = pair[1].split(' ')
                    if (len(freqs) < 2):
                        print "error in " + str(freqs)
                        break;
                    phrase = pair[0]
                    freq = [float(f) for f in freqs[:3]]
                    # print "%s - %g" % (phrase, freq)
                    d[phrase] = freq
                except:
                    print "exception in " + lines[i]
                    break;
    return d


def parse_enke_year(lines, start_index):
    """
    returns info from an enke-year
    :param lines: a list of lines representing the enke-file
    :param start_index: index of the first line in the year
    :return: dict, key = section-name, value = dict as returned by parse_enke_section()
    """
    d = {}
    for i in range(start_index, len(lines)):
        tok = lines[i].split(' ')
        # print tok
        # start marker for each year
        if (len(tok) >= 3 and tok[0] == 'year' and tok[2] == 'has'):
            break;
        # section start
        if (len(tok) >= 4 and tok[1] == '-' and tok[3] == '-'):
            sec_name = tok[0]
            #print sec_name
            if not sec_name == 'quotations':
                r = parse_enke_section(lines, i + 1)
                d[sec_name] = r
    return d


def parse_enke(text):
    """
    parse and return info from an enke-file
    :param text: The text of the file
    :return: dict, key = "year", value = dict as returned from parse_enke_year()
    """
    d = {}
    lines = text.split('\n')
    for i in range(0, len(lines)):
        tok = lines[i].split(' ')
        # start marker for each year
        if (len(tok) >= 3 and tok[0] == 'year' and tok[2] == 'has'):
            year_num = int(tok[1])
            print "----- %d -----" % year_num
            # if year_num == 2015:
            r = parse_enke_year(lines, i + 1)
            d[str(year_num)] = r
    return d


def parse_enke_file(fn):
    text = codecs.open(fn, 'r', 'utf-8').read()
    return parse_enke(text)


if __name__ == "__main__":
    fn = '/home/defgsus/prog/python/enkes_beste_nachrichten/heimat.enke'
    d = parse_enke_file(fn)
    print d
