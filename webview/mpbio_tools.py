class Abgeordneter:
    def __init__(self):
        self.name = ""
        self.url = ""
        self.img_url = ""
        self.occupation = ""
        self.birth = ""
        self.party = ""
        self.wahlkreis = ""
        self.period = ""
        self.gremium = ""
        self.statements = []
        self.member = []  # list of list of str
        for i in range(0, 4):
            self.member.append(list())

    def __str__(self):
        r = "name   [" + self.name + "]\n" \
            + "party  [" + self.party + "]\n" \
            + "occup  [" + self.occupation + "]\n" \
            + "birth  [" + self.birth + "]\n" \
            + "wahlkr [" + self.wahlkreis + "]\n" \
            + "gremiu [" + self.gremium + "]\n" \
            + "period [" + self.period + "]\n" \
            + "url    [" + self.url + "]\n" \
            + "imgurl [" + self.img_url + "]"
        k = 0
        for j in self.member:
            k = k + 1
            for i in j:
                r += "\nmemb" + str(k) + " [" + i + "]";

        for i in self.statements:
            r += "\nstat. [" + i + "]"
        return r

    def addStatement(self, st):
        # who knows, we can split them later anyway
        self.statements.append(st)
        """
            Adds str st to statements - splits statements at ';'
        sts = st.split(";")
        for i in sts:
            i = i.strip()
            if len(i):
                self.statements.append( i )
        """

    def concat(self, mlist):
        """Concats entries ending with ','
        Not really a member."""
        ret = []
        m = ""
        for i in mlist:
            if i.rfind(",") == len(i) - 1:
                m += " " + i
            else:
                # if len(m): self.member3.append( m )
                ret.append((m + " " + i).strip())
                m = ""
        return ret



def bio_save_xml(fn, people):
    """Saves a list of Abgeordneter to xml"""

    import xml.etree.ElementTree as ET

    root = ET.Element("bundes-bio")

    # create company nodes
    for c in people:
        P = ET.SubElement(root, "person")
        P.set("name", c.name)
        P.set("url", c.url)
        P.set("img_url", c.img_url)
        P.set("party", c.party)
        P.set("birth", c.birth)
        P.set("period", c.period)
        P.set("gremium", c.gremium)
        P.set("wahlkreis", c.wahlkreis)
        P.set("occupation", c.occupation)
        for s in c.statements:
            S = ET.SubElement(P, "statement")
            S.set("value", s)
        k = 0
        for i in c.member:
            for j in i:
                M = ET.SubElement(P, "member")
                M.set("type", str(k));
                M.set("value", j);
            k += 1

    # store
    tree = ET.ElementTree(root)
    tree.write(fn, encoding="utf-8", xml_declaration=True)