# -*- coding: utf-8 -*-

import csv, sqlite3

from fun_with_wiki import write_csv2


def connect(sqlite_file):
    """ Make connection to an SQLite database file """
    conn = sqlite3.connect(sqlite_file)
    c = conn.cursor()
    return conn, c
    

def close(conn):
    """ Commit changes and close connection to the database """
    # conn.commit()
    conn.close()
    
    
#takes a list of columns and returns them in a format ready to be inserted into an sql statement, if with_id is False, no key, if it is 'foreign' then foreign key, else primary key    
    
def create_schema(l_schema, with_id):
    if with_id == 'foreign':
        start = "(id INTEGER FOREIGN KEY"
        end = ")"
    elif with_id:
        start = "(id INTEGER PRIMARY KEY"
    else:
        start = "(%s" %(l_schema[0])
        l_schema = l_schema[1:]
    schema = start
    for col_name in l_schema:
        if col_name[-2 :] == '_i':
            schema = "%s, %s INTEGER" %(schema, col_name[:-2])
            
        else:
            schema = "%s, %s TEXT" %(schema, col_name)
         
    return """%s)""" %(schema) 
        
    
def create_table(curs, conn, name_table, str_schema):
    curs.execute("DROP TABLE IF EXISTS %s;" %(name_table))
    curs.execute("""CREATE TABLE %s %s;""" %(name_table, str_schema))
    conn.commit()
    
    
def create_table_schema(conn, curs, name_table, l_schema, with_id):
    str_schema = create_schema(l_schema, with_id)
    create_table(curs, conn, name_table, str_schema)
    conn.commit()

    
#create_db_schema('test3.db', 'extable',"ressort0 Text, year_issue1 TEXT, author2 TEXT, supertitle3 TEXT, title4 TEXT, description5 TEXT, text6 TEXT, date7 TEXT, url8 TEXT, len_text9 TEXT, len_unique_words10 TEXT, word_div_index11 TEXT, avg_word_len12 TEXT, word_freq_str13 TEXT", 2)

    
def insert_rows2(a_csv, conn, curs, name_table, l_schema):
    reader = list(csv.reader(open(a_csv, 'r')))[1:]
    schema = "(%s" %(l_schema[0])
    l_schema = l_schema[1:]
    for col_name in l_schema:
        schema = "%s, %s" %(schema, col_name)
    schema = "%s)" %(schema)
#    print schema, 'schema'
#    print
    str_marks = "(?"
    num_marks = len(l_schema)
    for i in range(num_marks):
        str_marks = "%s, ?" %(str_marks)
    str_marks += ")"
    print str_marks 
    for row in reader:
        if row[0] in ["spiele", "sport"]:
            continue
        to_db = [unicode(item, "utf8") for item in row]
        curs.execute("INSERT INTO %s %s VALUES %s;" %(name_table, schema, str_marks), to_db)
    conn.commit()
    
    
def insert_rows(conn, curs, l, name_table, schema, str_marks):
    sql = "INSERT INTO %s %s VALUES %s;" %(name_table, schema, str_marks)
#    print str_marks 
#    print sql
    curs.execute(sql, l)
    #conn.commit()
    
    
#conn, curs = connect('zeit.db')
   
    
def select_all_col(conn, curs, table):
    sql = "SELECT * from %s;" %(table)
    o = curs.execute(sql)
    l = o.fetchall()
    if len(l[0]) == 1:
        l = [i[0] for i in l]
    else:
        l = [list(i) for i in l]
    return l
    
    
def select_one_col(conn, curs, table, col):
    sql = "SELECT %s from %s;" %(col, table)
    o = curs.execute(sql)
    l = o.fetchall()
    l = [i[0] for i in l]
    return l
          
    
#print select_all_col(conn, curs, 'spec')
    
    
def select_one_marker(conn, curs, table, marker, l_col_to_sel, col_to_search):
    str_col_sel = l_col_to_sel[0]
    for i in l_col_to_sel[1:]:
        str_col_sel = '%s, %s' %(str_col_sel, i)
#    print str_col_sel, 'str_col_sel' 
    marker_format = "'%"+marker+"%'"
    sql = "select %s from %s where %s like %s" %(str_col_sel, table, col_to_search, marker_format)
    #print sql
    a = curs.execute(sql)
    l = a.fetchall()
#    for i in l:
#        print i
#    print len(l)
    return l
    
#conn, curs = connect('zeit.db') 
def select_precise_marker(conn, curs, table, marker, l_col_to_sel, col_to_search):
    str_col_sel = l_col_to_sel[0]
    for i in l_col_to_sel[1:]:
        str_col_sel = '%s, %s' %(str_col_sel, i)
#    print str_col_sel, 'str_col_sel' 
    sql = "select %s from %s where %s = '%s'" %(str_col_sel, table, col_to_search, marker)
    #print sql
    a = curs.execute(sql)
    l = a.fetchall()
#    for i in l:
#        print i
#    print len(l)
    return l    
    
#print select_precise_marker(conn, curs, 'zeit', '1946', ['id', 'title'], 'year')


def select_precise_and_like_marker(conn, curs, table, col_to_sel, precise_col_to_search, like_col_to_search, precise_marker, like_marker):
    sql = "select %s from %s where %s = '%s' and %s like '%s'" %(col_to_sel, table, precise_col_to_search, precise_marker, like_col_to_search, '%'+like_marker+'%')
    #print sql
    a = curs.execute(sql)
    l = a.fetchall()
#    for i in l:
#        print i
#    print len(l)
    return l 

    
def select_more(conn, curs, table, t_marker, l_col_to_sel, col_to_search, and_or):
    conn, curs = connect(db)
    l_marker = []
    str_marker = ''
    str_col_sel = l_col_to_sel[0]
    for i in l_col_to_sel[1:]:
        str_col_sel = '%s, %s' %(str_col_sel, i)
#    print str_col_sel, 'str_col_sel' 
    for i in t_marker:
        l_marker.append('%'+i+'%')
        str_marker = str_marker + ', %s' %(i)
    str_marker = str_marker[1:]
#    print str_marker, 'str_marker'
    sql = "select %s from %s where %s like ?" %(str_col_sel, table, col_to_search)
    sql_mid = "%s like ?" %(col_to_search)
#    print sql, 'sql0'
    for i in l_marker[1:]:
        sql = '%s %s %s' %(sql, and_or, sql_mid)
    sql = '%s;' %(sql)
#    print sql, 'sql'
    a = curs.execute(sql, l_marker)
    l = a.fetchall()
#    for i in l:
#        print i
#    print len(l)
    return l
    
    
def select_more2(conn, curs, table, t_marker, l_col_to_sel, col_to_search, and_or):
    l_marker = []
    str_marker = ''
    str_col_sel = l_col_to_sel[0]
    for i in l_col_to_sel[1:]:
        str_col_sel = '%s, %s' %(str_col_sel, i)
#    print str_col_sel, 'str_col_sel' 
    for i in t_marker:
        l_marker.append('%'+i+'%')
        str_marker = str_marker + ', %s' %(i)
    str_marker = str_marker[1:]
#    print str_marker, 'str_marker'
    sql = "select %s from %s where %s like ?" %(str_col_sel, table, col_to_search)
    sql_mid = "%s like ?" %(col_to_search)
#    print sql, 'sql0'
    for i in l_marker[1:]:
        sql = '%s %s %s' %(sql, and_or, sql_mid)
    sql = '%s;' %(sql)
#    print sql, 'sql'
    a = curs.execute(sql, l_marker)
    l = a.fetchall()
#    for i in l:
#        print i
#    print len(l)
    return l, str_marker
    
    
# selects cols based on a range of ids

#conn, curs = connect('zeit.db')

def select_in_range(conn, curs, table, l_col_to_sel, (start, end)):
    str_col_sel = l_col_to_sel[0]
    for i in l_col_to_sel[1:]:
        str_col_sel = '%s, %s' %(str_col_sel, i)
    sql = "SELECT %s FROM %s WHERE id >= %i and id < %i;" %(str_col_sel, table, start, end)
#    print sql
    stuff = curs.execute(sql)
    l = stuff.fetchall()
    return l
   
#print select_in_range(conn, curs, 'zeit', ['id', 'url'], (364000, 366000))

def select_id(conn, curs, table, the_id, l_col_to_sel):
    str_col_sel = l_col_to_sel[0]
    for i in l_col_to_sel[1:]:
        str_col_sel = '%s, %s' %(str_col_sel, i)
    sql = "SELECT %s FROM %s WHERE id = %i;" %(str_col_sel, table, the_id)
    stuff = curs.execute(sql)
    l = stuff.fetchall()
    return l
    

    
#makes a new table and fills it with the rows from a csv   
def csv_to_table(a_csv, db, name_table, l_schema, with_id):
    conn, curs = connect(db)
    create_db_schema(conn, curs, name_table, l_schema, with_id)
    insert_rows2(a_csv, conn, curs, name_table, l_schema)


#puts a csv into the colums of an existing table or creates a new one   
def csv_to_table2(a_csv, db, name_table, l_schema, with_id, table_exists):
    conn, curs = connect(db)
    if not table_exists:
        print 'yep'
        create_db_schema(conn, curs, name_table, l_schema, with_id)
    insert_rows2(a_csv, conn, curs, name_table, l_schema)
    
conn, curs = connect('zeit.db')    
def table_to_csv(conn, curs, name_table, a_csv):
    l_rows = select_all_col(conn, curs, name_table)
#    print l_rows[1]
    write_csv2(a_csv, l_rows)
    
#table_to_csv(conn, curs, 'pi', 'pi.csv')
    
    



    
    
    
