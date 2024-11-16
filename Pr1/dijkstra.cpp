#include <iostream>
#include <queue>
#include <algorithm>
using namespace std;

#define PA pair<int,int>

const int MAX=100005;
const long long INF=-1;

priority_queue<PA , vector<PA > , greater<PA > > q;
long long dist[MAX];
int from[MAX];
bool used[MAX]={false,};
vector<PA > graph[MAX];
int N,M;

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);

    cin>>N>>M;
    int x,y,w;
    for(int i=0;i<M;++i)
    {
        cin>>x>>y>>w;
        graph[x].push_back({y,w});
        graph[y].push_back({x,w});
    }

    fill(dist,dist+MAX,INF);
    fill(from,from+MAX,-1);
    fill(used,used+MAX,false);
    q.push(make_pair(0,1));
    dist[1]=0;
    while(!q.empty())
    {
        int v=q.top().second;
        int d=q.top().first;
        q.pop();
        if(used[v])continue;
        used[v]=true;
        for(int i=0;i<graph[v].size();++i)
        {
            if(!used[graph[v][i].first] && (dist[graph[v][i].first]==INF || dist[graph[v][i].first]>dist[v]+graph[v][i].second))
            {
               dist[graph[v][i].first]=dist[v]+graph[v][i].second ;
               from[graph[v][i].first]=v;
               q.push({dist[graph[v][i].first],graph[v][i].first});
            }
        }
    }

    if(dist[N]==INF)
    {
        cout<<-1<<endl;
        return 0;
    }

    vector<int> qq;
    int v=N;
    while(v!=-1)
    {
        qq.push_back(v);
        v=from[v];
    }

    reverse(qq.begin(),qq.end());

    for(int x:qq)
        cout<<x<<' ';
    cout<<endl;

    return 0;
}