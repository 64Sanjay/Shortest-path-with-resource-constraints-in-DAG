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
struct LabelComparator
{
    bool operator()(const Label &l1, const Label &l2) const
    {
        return l1.r.cost > l2.r.cost; // Compare costs in reverse order for minimum cost extraction
    }
};
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
        int c;
        cin >> c;
        Resources re(numResources, c);
        for (int j = 0; j < numResources; j++)
            cin >> re.resources[j];
        DAG[s].r.push_back(re);
    }
    // cout << "Enter Restrictions :";
    Resources restricted(numResources, INT_MAX);
    for (int i = 0; i < numResources; i++)
        cin >> restricted.resources[i];
    int source;
    int dest;
    cin >> source;
    cin >> dest;
    auto start = std::chrono::high_resolution_clock::now();
    priority_queue<Label, vector<Label>, LabelComparator> pq;
    Label first_label(source);
    countLabels++;
    pq.push(first_label);
    map<int, vector<Label>> mp;
    mp[source].push_back(first_label);
    Label ans = first_label;
    ans.r.cost = INT_MAX;
    while (!pq.empty())
    {
        Label current_label = pq.top();
        pq.pop();
        // printf("Extracted\n");
        // printLabel(current_label);
        int i = current_label.topNode;
        vector<Label> labels_on_i = mp[i];
        int numLabels = labels_on_i.size();
        bool sat = false;
        for (auto j : mp[i])
            if (j == current_label)
                sat = true;
        if (!sat)
            continue;
        if (!current_label.dominated)
        {
            for (int j = 0; j < numLabels; j++)
            {
                for (int k = 0; k < numLabels; k++)
                {
                    if (k != j && dominance(labels_on_i[j].r, labels_on_i[k].r))
                    {
                        if (labels_on_i[j] == current_label)
                        {
                            current_label.dominated = true;
                        }
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
            mp[i] = labels_on_i;
        }
        if (!current_label.dominated)
        {
            current_label.processed = true;
            int i = current_label.topNode;
            for (int j = 0; j < DAG[i].adj.size(); j++)
            {
                Label newLabel(DAG[i].adj[j], current_label, DAG[i].r[j]);

                if (feasible(newLabel.r, restricted))
                {
                    countLabels++;
                    // printf("Added :\n");
                    // printLabel(newLabel);
                    mp[DAG[i].adj[j]].push_back(newLabel);
                    pq.push(newLabel);
                    if (DAG[i].adj[j] == dest && ans.r.cost > newLabel.r.cost)
                        ans = newLabel;
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
    // cout << "Done\n";
    printLabel(ans);
    cout << "Number of labels :" << countLabels << endl;
}