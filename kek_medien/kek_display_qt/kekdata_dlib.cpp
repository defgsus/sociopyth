/** @file kekdata_dlib.cpp

    @brief all dlib specific implementation

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 2/12/2015</p>
*/

#include <cassert>

//#include <dlib/directed_graph/directed_graph_kernel_1.h>
//#include <dlib/svm_threaded.h>
#include <dlib/graph_utils.h>
#include <dlib/clustering.h>

#include <QDebug>

#include "kekdata.h"



void KekData::getClusters_()
{
    qDebug() << "clustering" << compmap_.size() << " entities...";

    // transfer the edge data

    typedef std::vector<dlib::ordered_sample_pair> Edges;

    Edges edges;

    for (auto const & i : compmap_)
    {
        Company * c = i.second.get();
        for (const Share & s : c->shares)
        {
            edges.push_back(dlib::ordered_sample_pair(
                                s.company->index, c->index, s.percent));
//            edges.push_back(dlib::ordered_sample_pair(
//                                c->index, s.company->index, s.percent));
        }
    }

    // precondition for algo
    std::sort(edges.begin(), edges.end(), dlib::order_by_index<dlib::ordered_sample_pair>);

    // get labels
    std::vector<unsigned long> labels;

    unsigned long num = dlib::chinese_whispers(edges, labels, 1000);
    //unsigned long num = dlib::newman_cluster(edges, labels, 0.1, 4);

    qDebug() << "found" << num << "clusters in" << compmap_.size() << "entities with"
             << edges.size() << "edges";

    assert(labels.size() >= compmap_.size());

    // get cluster labels
    std::multimap<unsigned long, Company*> cmap;
    for (auto const & i : compmap_)
    {
        Company * c = i.second.get();
        c->clusterId = labels[c->index];
        c->clusterMembers = 0;
        cmap.insert(std::make_pair(c->clusterId, c));
    }

    // count members per cluster
    for (auto const & i : compmap_)
    {
        Company * c = i.second.get();

        auto j = cmap.find(c->clusterId);
        while (j != cmap.end() && j->second->clusterId == c->clusterId)
        {
            j->second->clusterMembers++;
            ++j;
        }
    }
}

