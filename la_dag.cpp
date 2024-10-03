#include <bits/stdc++.h>
#include <chrono>

using namespace std;

int numResources = 1;
int numNodes = 11;
int numEdges = 18;
int countLabels = 0;
class Resources
{
public:
    double cost;
    vector<double> resources;
    Resources() {}
    Resources(int numResources, int c)
    {
        resources.resize(numResources);
        cost = c;
    }
    bool operator==(const Resources &other) const
    {
        return (cost == other.cost) && (resources == other.resources);
    }
};

class Node
{
public:
    vector<int> adj;
    vector<Resources> r;
};
bool dominance(Resources l1, Resources l2)
{
    bool greater = false;
    for (int i = 0; i < numResources; i++)
        if (l1.resources[i] < l2.resources[i])
            return false;
        else if (l1.resources[i] > l2.resources[i])
            greater = true;
    if (greater)
        return true;
    return l1.cost >= l2.cost;
}
class Label
{
public:
    int topNode;
    vector<int> path;
    Resources r;
    bool dominated = false;
    bool processed = false;
    Label(int top, Label l, Resources re)
    {
        topNode = top;
        r = l.r;
        path = l.path;
        path.push_back(top);
        r.cost += re.cost;
        for (int i = 0; i < numResources; i++)
            r.resources[i] += re.resources[i];
    }
    Label(int top)
    {
        topNode = top;
        r.resources.resize(numResources);
        for (int i = 0; i < numResources; i++)
            r.resources[i] = 0;
        r.cost = 0;
        path.push_back(top);
    }
    bool operator==(const Label &other) const
    {
        return (topNode == other.topNode) && (path == other.path) && (r == other.r) && (dominated == other.dominated);
    }
    bool operator<(const Label &other) const
    {
        for (int i = 0; i < numResources; i++)
        {
            if (r.resources[i] < other.r.resources[i])
                return true;
            if (r.resources[i] > other.r.resources[i])
                return false;
        }
        return r.cost < other.r.cost;
    }
};
bool feasible(Resources x, Resources restricted)
{
    for (int i = 0; i < numResources; i++)
        if (x.resources[i] > restricted.resources[i])
            return false;
    return true;
}
void printLabel(Label l)
{
    for (auto i : l.path)
        cout << i << " ";
    cout << endl;
    cout << l.r.cost << endl;
    for (auto i : l.r.resources)
        cout << i << " ";
    cout << endl;
    cout << l.dominated << endl;
    cout << l.processed << endl
         << endl;
}
void topologyOrder(vector<Node> &DAG, int i, int val, map<int, int> &mp)
{
    mp[i] = max(mp[i], val);
    for (int j = 0; j < DAG[i].adj.size(); j++)
        topologyOrder(DAG, DAG[i].adj[j], mp[i] + 1, mp);
}
int main()
{
    // cin >> numResources >> numNodes;
    vector<Node> DAG(numNodes);
    // cin>>numEdges;
    for (int i = 0; i < numEdges; i++)
    {
        int s, d;
        cin >> s >> d;
        DAG[s].adj.push_back(d);
        double c;
        cin >> c;
        Resources re(numResources, c);
        for (int j = 0; j < numResources; j++)
            cin >> re.resources[j];
        DAG[s].r.push_back(re);
    }
    cout << "Enter Restrictions :";
    Resources restricted(numResources, INT_MAX);
    for (int i = 0; i < numResources; i++)
        cin >> restricted.resources[i];
    int source;
    int dest;
    cin >> source;
    cin >> dest;
    auto start = std::chrono::high_resolution_clock::now();
    map<int, int> mp;
    topologyOrder(DAG, source, 0, mp);
    vector<int> orderVector(mp.size());
    for (auto i : mp)
    {
        // cout << i.first << " " << i.second << endl;
        orderVector[i.second] = i.first;
    }
    map<int, set<Label>> sI;
    Label first_label(source);
    countLabels++;
    sI[source].insert(first_label);
    Label ans = first_label;
    ans.r.cost = INT_MAX;
    for (int i : orderVector)
    {
        if (i == dest)
            break;
        vector<Label> labels_on_i;
        for (Label l : sI[i])
        {
            labels_on_i.push_back(l);
        }
        for (int j = labels_on_i.size() - 1; j >= 0; j--)
        {
            for (int k = j - 1; k >= 0; k--)
            {
                if (dominance(labels_on_i[j].r, labels_on_i[k].r))
                {
                    labels_on_i[j].dominated = true;
                    break;
                }
            }
        }

        for (int j = 0; j < labels_on_i.size(); j++)
        {
            if (!labels_on_i[j].dominated)
            {
                for (int k = 0; k < DAG[i].adj.size(); k++)
                {
                    Label newLabel(DAG[i].adj[k], labels_on_i[j], DAG[i].r[k]);

                    if (feasible(newLabel.r, restricted))
                    {
                        countLabels++;
                        // printf("Added :\n");
                        // printLabel(newLabel);
                        sI[DAG[i].adj[k]].insert(newLabel);
                        if (DAG[i].adj[k] == dest && ans.r.cost > newLabel.r.cost)
                            ans = newLabel;
                    }
                }
            }
        }
    }
    // Stop the clock
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the elapsed time
    std::chrono::duration<double> duration = end - start;
    double execution_time = duration.count();

    // Output the execution time
    std::cout << "Execution Time: " << execution_time << " seconds" << std::endl;
    // printLabel(ans);
    cout << "Labels created :" << countLabels << endl;
}