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
    Label(Label l1, Label l2)
    {
        topNode = l2.path[0];
        r = l1.r;
        r.cost += l2.r.cost;
        for (int i = 0; i < numResources; i++)
            r.resources[i] += l2.r.resources[i];
        path = l1.path;
        for (int i = l2.path.size() - 2; i >= 0; i--)
            path.push_back(l2.path[i]);
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
void topologyOrder(vector<vector<Resources>> &DAG, int i, int val, map<int, int> &mp)
{
    mp[i] = max(mp[i], val);
    for (int j = 0; j < numNodes; j++)
        if (DAG[i][j].cost != INT_MAX)
            topologyOrder(DAG, j, mp[i] + 1, mp);
}
int main()
{
    // cin >> numResources >> numNodes;
    Resources hr(numResources, INT_MAX);
    vector<vector<Resources>> DAG(numNodes, vector<Resources>(numNodes, hr));
    // cin>>numEdges;
    for (int i = 0; i < numEdges; i++)
    {
        int s, d;
        cin >> s >> d;
        double c;
        cin >> c;
        Resources re(numResources, c);
        for (int j = 0; j < numResources; j++)
            cin >> re.resources[j];
        DAG[s][d] = re;
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
    map<int, set<Label>> forward;
    map<int, set<Label>> backward;
    Label first_label(source);
    countLabels++;
    Label last_label(dest);
    countLabels++;
    forward[source].insert(first_label);
    backward[dest].insert(last_label);
    int end = mp.size();
    for (int i = 0; i < mp.size(); i++)
        if (orderVector[i] == dest)
            end = dest + 1;
    int milestone = (end - source) / 2;
    for (int i = 0; i < milestone; i++)
    {
        vector<Label> labels_on_i;
        for (Label l : forward[orderVector[i]])
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
        for (auto it = labels_on_i.begin(); it != labels_on_i.end();)
        {
            if (it->dominated)
            {
                // printf("Deleted\n");
                // printLabel(*it);
                it = labels_on_i.erase(it);
            }
            else
            {
                ++it;
            }
        }
        set<Label> newSet;
        for (Label l : labels_on_i)
            newSet.insert(l);
        forward[orderVector[i]] = newSet;
        for (int j = 0; j < labels_on_i.size(); j++)
        {
            for (int k = 0; k < numNodes; k++)
            {
                if (DAG[orderVector[i]][k].cost != INT_MAX)
                {
                    Label newLabel(k, labels_on_i[j], DAG[orderVector[i]][k]);
                    if (feasible(newLabel.r, restricted))
                    {
                        countLabels++;
                        // printf("Added :\n");
                        // printLabel(newLabel);
                        forward[k].insert(newLabel);
                    }
                }
            }
        }
    }

    for (int i = end - 1; i > milestone; i--)
    {
        vector<Label> labels_on_i;
        for (Label l : backward[orderVector[i]])
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
        for (auto it = labels_on_i.begin(); it != labels_on_i.end();)
        {
            if (it->dominated)
            {
                // printf("Deleted\n");
                // printLabel(*it);
                it = labels_on_i.erase(it);
            }
            else
            {
                ++it;
            }
        }
        set<Label> newSet;
        for (Label l : labels_on_i)
            newSet.insert(l);
        backward[orderVector[i]] = newSet;
        for (int j = 0; j < labels_on_i.size(); j++)
        {
            if (!labels_on_i[j].dominated)
            {
                for (int k = 0; k < numNodes; k++)
                {
                    if (DAG[k][orderVector[i]].cost != INT_MAX)
                    {
                        Label newLabel(k, labels_on_i[j], DAG[k][orderVector[i]]);
                        if (feasible(newLabel.r, restricted))
                        {
                            countLabels++;
                            // printf("Added :\n");
                            // printLabel(newLabel);
                            backward[k].insert(newLabel);
                        }
                    }
                }
            }
        }
    }
    Label ans = first_label;
    ans.r.cost = INT_MAX;
    for (int i = milestone; i < end; i++)
    {
        for (Label fl : forward[orderVector[i]])
        {
            for (Label bl : backward[orderVector[i]])
            {
                Label newLabel(fl, bl);

                if (feasible(newLabel.r, restricted))
                {
                    countLabels++;
                    // printf("Joined :\n");
                    // printLabel(newLabel);
                    if (ans.r.cost > newLabel.r.cost)
                        ans = newLabel;
                }
            }
        }
    }
    auto endT = std::chrono::high_resolution_clock::now();

    // Calculate the elapsed time
    std::chrono::duration<double> duration = endT - start;
    double execution_time = duration.count();

    // Output the execution time
    std::cout << "Execution Time: " << execution_time << " seconds" << std::endl;
    // printLabel(ans);
    cout << countLabels << endl;
}