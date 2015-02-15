/** @file detailview.cpp

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 15.02.2015</p>
*/

#include <QTextStream>
#include <QDesktopServices>
#include <QSyntaxHighlighter>
#include <QRegExp>

#include "detailview.h"

class Highlighter : public QSyntaxHighlighter
{
    QStringList patterns_;

public:
    Highlighter(QObject * p) : QSyntaxHighlighter(p) { }
    Highlighter(QTextDocument * d) : QSyntaxHighlighter(d) { }

    void setPatterns(const QStringList& s) { patterns_ = s; rehighlight(); }

    void highlightBlock(const QString &text) override
    {
        QTextCharFormat fm;
        fm.setFontWeight(QFont::Bold);
        fm.setBackground(Qt::gray);
        for (auto & p : patterns_)
        {
            QRegExp expression;
            expression.setPatternSyntax(QRegExp::FixedString);
            expression.setPattern(p);
            expression.setCaseSensitivity(Qt::CaseInsensitive);

            int index = text.indexOf(expression);
            while (index >= 0)
            {
                int length = expression.matchedLength();
                setFormat(index, length, fm);
                index = text.indexOf(expression, index + length);
            }
        }
    }
};









DetailView::DetailView(QWidget *parent) :
    QTextBrowser(parent)
{
    setOpenLinks(false);

    highlighter_ = new Highlighter(document());

    connect(this, SIGNAL(anchorClicked(QUrl)),
            this, SLOT(onAnchorClicked_(QUrl)));
}

void DetailView::onAnchorClicked_(const QUrl &url)
{
    QString link = url.toString();
    /*if (link.startsWith("comp:"))
        emit companySelected(link.mid(5));
    else*/ if (link.startsWith("http"))
        QDesktopServices::openUrl(url);
}


void DetailView::setPerson(const BioData::Person * p)
{
    clear();

    QString text;
    QTextStream s(&text);

    s << "<html><h1>" << p->name << "</h1>\n"
      << "<ul><li>" << p->wahlkreis << "</li>\n"
      << "<li>" << p->birth << "</li>\n"
      << "<li>" << p->occupation << "</li>\n"
      << "<li><a href=\"" << p->url << "\">bundestag.de</a></li>\n"
      << "</ul>";

    if (!p->statements.empty())
    {
        s << "<p><h3>" << tr("Biographie") << ":</h3><ul>\n";

        for (auto & st : p->statements)
        {
            s << "<li>" << st << "</li>\n";
        }
        s << "</ul></p>\n";
    }

    if (!p->memberships.empty())
    {
        s << "<p><h3>" << tr("Funktionen") << ":</h3><ul>\n";

        int k=-1;
        for (const QStringList& l : p->memberships)
        {
            ++k;
            if (l.isEmpty())
                continue;

            s << "<li><h4>";

            switch (k)
            {
                case 0: s << tr("ordentliches Mitglied"); break;
                case 1: s << tr("stellvertretendes Mitglied"); break;
                case 2: s << tr("Mitgliedschaften / Beteiligungen"); break;
                case 3: s << tr("Funktionen vor dem Bundestag"); break;
            }
            s << "</h4><ul>";

            for (auto & st : l)
                s << "<li>" << st << "</li>\n";

            s << "</ul></li>\n";
        }

        s << "</ul></p>\n";
    }

    setHtml(text);
    updateFilter_();
}


void DetailView::setTextFilter(const QString &filters)
{
    textFilters_.clear();
    auto list = filters.split(";");
    for (QString s : list)
    {
        s = s.simplified();
        if (!s.isEmpty())
            textFilters_.append(s);
    }
    updateFilter_();
}


void DetailView::updateFilter_()
{
    highlighter_->setPatterns(textFilters_);
    /*
    QString text = src_;

    for (auto & filter : textFilters_)
    {
        text.replace(filter, "<strong>" + filter + "</strong>", Qt::CaseInsensitive);
    }

    setHtml(text);
    */
}
