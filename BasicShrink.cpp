#include <vector>
#include <fstream>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <list>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <climits>
using namespace std;

// g++ -DMOVIELENS -O2 -o BasicShrink_Movielens BasicShrink.cpp
// ./BasicShrink_Movielens 14 2 5 10 10

#if defined(MOVIELENS)
char* HIN_PATH = (char*)"Movielens/graph_Movielens.txt";
char* VERTEX_PATH = (char*)"Movielens/vertices_Movielens.txt";
char* OUTPUT_FILE = (char*)"Output/Movielens_shrink-basic.txt";
enum VertexType {
    User,
    Age,
    Occupation,
    Genre,
    Movie
};
vector<VertexType> metaPath = {User, Movie, User};
VertexType type1 = User;
VertexType type2 = Movie;
vector<int> involve_edge = {1};
#elif defined(DBLP)
char* HIN_PATH = (char*)"dblp/edges.txt";
char* VERTEX_PATH = (char*)"dblp/vertices.txt";
char* OUTPUT_FILE = (char*)"Output/dblp_shrink-basic.txt";
enum VertexType {
    Author,
    Paper,
    Venues
};
vector<VertexType> metaPath = {Author, Paper, Author};
VertexType type1 = Author;
VertexType type2 = Paper;
vector<int> involve_edge = {1, 3};
#elif defined(FOURSQUARE)
char* HIN_PATH = (char*)"foursquare/edges.txt";
char* VERTEX_PATH = (char*)"foursquare/vertices.txt";
char* OUTPUT_FILE = (char*)"Output/foursquare_shrink-basic.txt";
enum VertexType {
    User,
    Venues,
    VenuesType
};
vector<VertexType> metaPath = {User, Venues, User};
VertexType type1 = User;
VertexType type2 = Venues;
vector<int> involve_edge = {1};
#elif defined(AMAZON)
char* HIN_PATH = (char*)"amazon/edges.txt";
char* VERTEX_PATH = (char*)"amazon/vertices.txt";
char* OUTPUT_FILE = (char*)"Output/amazon_shrink-basic.txt";
enum VertexType {
    User,
    Product,
    Brand,
    Category
};
vector<VertexType> metaPath = {User, Product, User};
VertexType type1 = User;
VertexType type2 = Product;
vector<int> involve_edge = {1, 2};
#elif defined(YAGO)
char* HIN_PATH = (char*)"yago/edges.txt";
char* VERTEX_PATH = (char*)"yago/vertices.txt";
char* OUTPUT_FILE = (char*)"Output/yago_shrink-basic.txt";
enum VertexType {
    Person,
    airport,
    imports_wordnet,
    CreativeWork,
    country,
    college,
    web,
    Organization,
    Award,
    exports_wordnet,
    Gender,
    Place,
    team,
    currency,
    knowthing,
    subject,
    language,
    Event,
    personalThing,
    instrument
};
vector<VertexType> metaPath = {Person, Place, Person};
VertexType type1 = Person;
VertexType type2 = Place;
vector<int> involve_edge = {0};
#elif defined(DBLP20)
char* HIN_PATH = (char*)"dblp20/edges.txt";
char* VERTEX_PATH = (char*)"dblp20/vertices.txt";
char* OUTPUT_FILE = (char*)"Output/dblp20_shrink-basic.txt";
enum VertexType {
    Author,
    Paper,
    Venues
};
vector<VertexType> metaPath = {Author, Paper, Author};
VertexType type1 = Author;
VertexType type2 = Paper;
vector<int> involve_edge = {1, 3};
#elif defined(DBLP40)
char* HIN_PATH = (char*)"dblp40/edges.txt";
char* VERTEX_PATH = (char*)"dblp40/vertices.txt";
char* OUTPUT_FILE = (char*)"Output/dblp40_shrink-basic.txt";
enum VertexType {
    Author,
    Paper,
    Venues
};
vector<VertexType> metaPath = {Author, Paper, Author};
VertexType type1 = Author;
VertexType type2 = Paper;
vector<int> involve_edge = {1, 3};
#elif defined(DBLP60)
char* HIN_PATH = (char*)"dblp60/edges.txt";
char* VERTEX_PATH = (char*)"dblp60/vertices.txt";
char* OUTPUT_FILE = (char*)"Output/dblp60_shrink-basic.txt";
enum VertexType {
    Author,
    Paper,
    Venues
};
vector<VertexType> metaPath = {Author, Paper, Author};
VertexType type1 = Author;
VertexType type2 = Paper;
vector<int> involve_edge = {1, 3};
#elif defined(DBLP80)
char* HIN_PATH = (char*)"dblp80/edges.txt";
char* VERTEX_PATH = (char*)"dblp80/vertices.txt";
char* OUTPUT_FILE = (char*)"Output/dblp80_shrink-basic.txt";
enum VertexType {
    Author,
    Paper,
    Venues
};
vector<VertexType> metaPath = {Author, Paper, Author};
VertexType type1 = Author;
VertexType type2 = Paper;
vector<int> involve_edge = {1, 3};
#elif defined(DBLPCASE)
char* HIN_PATH = (char*)"dblp_case/edges.txt";
char* VERTEX_PATH = (char*)"dblp_case/vertices.txt";
char* OUTPUT_FILE = (char*)"Output/dblp_case_shrink-basic.txt";
enum VertexType {
    Author,
    Paper,
    Venues
};
vector<VertexType> metaPath = {Author, Paper, Author};
VertexType type1 = Author;
VertexType type2 = Paper;
vector<int> involve_edge = {1, 3};
#else
#error "-D shoud be defined in (MOVIELENS; DBLP; FOURSQUARE; AMAZON; YAGO)"
#endif

typedef unordered_map<VertexType, int> threshold;

typedef struct {
	vector<int> neighbors;
    int deg;
    int instanceNum;
	VertexType type;
}vertice;

typedef struct {
	int n;
	int m;
	int t;
	vector<vertice> v;
}graph;

typedef struct {
    // shrinkStack[i].PeelingVertexId  选择的节点id
    int selectedVertexId;
    // shrinkStack[i].delHsize     删除之后Hsize减少了多少
    int delHsize;
    // shrinkStack[i].delTargetNum     删除之后delTargetNum减少了多少
    int delTargetNum;
    // shrinkStack[i].Candidate  子图Candidate
    vector<int> Candidate;
    unordered_set<int> isInCandidate;
    // shrinkStack[i].delDegree   删除后，deg下降的节点：deg下降了多少
    std::unordered_map<int, int> delDegree;
    // shrinkStack[i].delMetaGraph: 删除后，MetaGraph变化量
    std::unordered_map<int, std::unordered_map<int, int>> delMetaGraph;
    // shrinkStack[i].delInstanceNum   删除后，instanceNum下降的节点：instanceNum下降了多少
    std::unordered_map<int, int> delInstanceNum;

}shrink_stack_object;
vector<shrink_stack_object> shrink_stack;
int shrink_stack_size = 0;

graph g;
vector<int> H;
int target_vertex_number;
int Hsize = 0;
// meta-path instances number between target vertex (only record larger than k)
// example: 
// {
//     1: {{5, 1}, {3, 2}},
//     5: {{1, 1}},
//     3: {{1, 2}}
// }
unordered_map<int, unordered_map<int, int>> metaGraph; 
// 图密集时，这个vector<vector<int>>效果可能更好
// vector<vector<int>> metaGraph;
threshold degreeThreshold;
int k;
int size_constraint;
int queryVertexId;
int metaPathLength = metaPath.size();
VertexType targetType = metaPath[0];
chrono::time_point<chrono::high_resolution_clock> start_time;
chrono::time_point<chrono::high_resolution_clock> end_time;
chrono::duration<long long, milli> duration_time;
chrono::duration<long long, milli> dpcore_time;
int back_track_times = 0;
int iteration_times = 0;

void load_graph();
void DPcore();
void updateHConnect();
int Psupport(int id);
void Impact(int delVertexId, set<int>& ImpactSet);
bool hasInstance(int id);
void dfsImpact(int delVertexId, int delIndex, set<int>& Impact);
bool dfsHasInstance(int id, int vertexIndex);
void joinedImpact(int delVertexId, int delIndex, set<int>& Impact);
bool joinedHasInstance(int id, int vertexIndex);
void bfsFindPathInstances(int delVertexId, const vector<VertexType>& path, vector<vector<int>>& instances);
void delVertex(int id);
bool isValInVector(int val, const vector<int>& vec);
void printParameter();
void printH();
void constructMetaGraphfrom(int id);
void delVertexAndUpdateQ(int id, vector<int>& Q, unordered_set<int>& isInQ);
void basic_shrink();
bool isPromingBranch();
void DPcore_lb();
int estimate_Psupport(int id);

int main(int argc, char* argv[]) {
    queryVertexId = atoi(argv[1]);
    degreeThreshold[type1] = atoi(argv[2]);
    degreeThreshold[type2] = atoi(argv[3]);
    k = atoi(argv[4]);
    size_constraint = atoi(argv[5]);
    load_graph();
    start_time = chrono::high_resolution_clock::now();
    DPcore_lb();
    end_time = chrono::high_resolution_clock::now();
    dpcore_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    start_time = chrono::high_resolution_clock::now();
    basic_shrink();
    end_time = chrono::high_resolution_clock::now();
    printParameter();
    printH();
    return 0;
}

void load_graph() {
    FILE* infp = fopen(HIN_PATH, "r");
	if (infp == NULL) {
		fprintf(stderr, "Error: could not open input file: %s.\n Exiting ...\n", HIN_PATH);
		exit(1);
	}
	
	int items_read = fscanf(infp, "%d %d %d\n", &(g.n), &(g.m), &(g.t));
    if (items_read != 3) {
        fprintf(stderr, "Error: Failed to read three integers.\n");
    }
    int u, v, type;
    g.v.resize(g.n);
    while (fscanf(infp, "%d %d %d\n", &u, &v, &type) != EOF) {
        if (isValInVector(type, involve_edge)) {
            g.v[u].neighbors.push_back(v);
            g.v[v].neighbors.push_back(u);
        }
	}
	fclose(infp);

    infp = fopen(VERTEX_PATH, "r");
    if (infp == NULL) {
		fprintf(stderr, "Error: could not open input file: %s.\n Exiting ...\n", VERTEX_PATH);
		exit(1);
	}
		
	for (int i = 0; i < g.n; i++)
	{
		items_read = fscanf(infp, "%d\n", &type);
        if (items_read != 1) {
            fprintf(stderr, "Error: Failed to read one integers.\n");
        }
		if (type == type1 || type == type2) {
            g.v[i].type = static_cast<VertexType>(type);
            g.v[i].deg = g.v[i].neighbors.size();
            g.v[i].instanceNum = 0;
        }
	}
	fclose(infp);
	return ;
}

void DPcore() {
    // cout << "Ready to run DPcore..." << endl;
    vector<int> Q;
    H.resize(g.n);
    // 去掉与q不连通的节点
    updateHConnect();
    for (int i = 0; i < Hsize; i++)
    {
        if (g.v[H[i]].deg < degreeThreshold[g.v[H[i]].type]) {
            Q.push_back(H[i]);
        }
    }
    int iteration = 0;
    while (!Q.empty())
    {
        iteration++;
        int id = Q.back();
        Q.pop_back();
        for (int i = 0; i < g.v[id].deg; i++) {
            int neighbor = g.v[id].neighbors[i];
            if (g.v[neighbor].deg == degreeThreshold[g.v[neighbor].type]) {
                Q.push_back(neighbor);
            }
        }
        delVertex(id);
    }
    // cout << "degree peeling iteration: " << iteration << endl;

    unordered_set<int> isInQ;
    for (int i = 0; i < Hsize; i++) {
        if (g.v[H[i]].type == targetType) {
            constructMetaGraphfrom(H[i]);
            if (Psupport(H[i]) < k) {
                Q.push_back(H[i]);
                isInQ.insert(H[i]);
            }
        }
    }
    for (int i = 0; i < Hsize; i++) {
        if (g.v[H[i]].type != targetType && g.v[H[i]].instanceNum == 0) {
            Q.push_back(H[i]);
            isInQ.insert(H[i]);
        }
    }

    // cout << "After construct MetaGraph... \nP-support and instanceNum peel Qsize: " << Q.size() << endl;

    iteration = 0;
    while (!Q.empty()) {
        iteration++;
        int id = Q.back();
        Q.pop_back();
        isInQ.erase(id);
        for (int i = 0; i < g.v[id].deg; i++) {
            int neighbor = g.v[id].neighbors[i];
            if (g.v[neighbor].deg == degreeThreshold[g.v[neighbor].type]) {
                Q.push_back(neighbor);
                isInQ.insert(neighbor);
            }
        }
        delVertexAndUpdateQ(id, Q, isInQ);
        delVertex(id);
    }
    updateHConnect();
    return ;
}

int Psupport(int id) {
    int PSupport = 0;
    for (const auto& pair : metaGraph[id]){
        PSupport = (pair.second > PSupport) ? pair.second : PSupport;
    }
    return PSupport;
}

bool hasInstance(int id) {
    VertexType delVertexType = g.v[id].type;
    vector<int> delVertexPos;
    for (int i = 0; i < metaPathLength; ++i) {
        if (metaPath[i] == delVertexType) {
            delVertexPos.push_back(i);
        }
    }
    for (const auto& delIndex : delVertexPos) {
        if (delIndex == 0 || delIndex == metaPathLength - 1) {
            if (dfsHasInstance(id, delIndex)) {
                return true;
            }
        }
        else {
            vector<vector<int>> joinedInstances;
            if (joinedHasInstance(id, delIndex)) {
                return true;
            }
        }
    }
    return false;
}

void Impact(int delVertexId, set<int>& ImpactSet) {
    for (int i = 0; i < g.v[delVertexId].deg; i++) {
        ImpactSet.insert(g.v[delVertexId].neighbors[i]);
    }
    VertexType delVertexType = g.v[delVertexId].type;
    vector<int> delVertexPos;
    for (int i = 0; i < metaPathLength; ++i) {
        if (metaPath[i] == delVertexType) {
            delVertexPos.push_back(i);
        }
    }
    for (const auto& delIndex : delVertexPos) {
        if (delIndex == 0 || delIndex == metaPathLength - 1) {
            dfsImpact(delVertexId, delIndex, ImpactSet);
        }
        else {
            vector<vector<int>> joinedInstances;
            joinedImpact(delVertexId, delIndex, ImpactSet);
        }
    }
}

void dfsImpact(int delVertexId, int delIndex, set<int>& ImpactSet) {
    vector<VertexType> path;
    if (delIndex == metaPath.size()-1) {
        vector<VertexType> metaPathReverse(metaPath.begin(), metaPath.end());
        reverse(metaPathReverse.begin(), metaPathReverse.end());
        path = metaPathReverse;
    }
    else {
        path = metaPath;
    }
    list<vector<int>> stack;
    size_t metaPathLength = metaPath.size();
    unordered_set<int> visited;
    
    // {vertexId, pos}
    stack.push_back({delVertexId, 0});
    visited.insert(delVertexId);
    int pos = 0;

    
    while (!stack.empty()) {
        if (stack.size() == metaPathLength) {
            for (auto& pair : stack) {
                ImpactSet.insert(pair[0]);
            }

            // 回溯
            pos = stack.back()[1] + 1;
            visited.erase(stack.back()[0]);
            stack.pop_back();
        } 
        else {
            vector<int> currentPair = stack.back();
            int currentVertexId = currentPair[0];
            int flag = true;

            if (pos >= g.v[currentVertexId].deg) {
                pos = stack.back()[1] + 1;
                visited.erase(stack.back()[0]);
                stack.pop_back();
                continue;
            }
            
            while (g.v[g.v[currentVertexId].neighbors[pos]].type != metaPath[stack.size()] 
                    || visited.find(g.v[currentVertexId].neighbors[pos]) != visited.end()) {
                ++pos;
                if (pos >= g.v[currentVertexId].deg) {
                    pos = stack.back()[1] + 1;
                    visited.erase(stack.back()[0]);
                    stack.pop_back();
                    flag = false;
                    break;
                }
            }

            if (flag) {
                stack.push_back({g.v[currentVertexId].neighbors[pos], pos});
                visited.insert(g.v[currentVertexId].neighbors[pos]);
                pos = 0;
            }
        }
    }
}

bool dfsHasInstance(int id, int vertexIndex) {
    vector<VertexType> path;
    if (vertexIndex == metaPath.size()-1) {
        vector<VertexType> metaPathReverse(metaPath.begin(), metaPath.end());
        reverse(metaPathReverse.begin(), metaPathReverse.end());
        path = metaPathReverse;
    }
    else {
        path = metaPath;
    }
    list<vector<int>> stack;
    size_t metaPathLength = metaPath.size();
    unordered_set<int> visited;
    
    // {vertexId, pos}
    stack.push_back({id, 0});
    visited.insert(id);
    int pos = 0;

    
    while (!stack.empty()) {
        if (stack.size() == metaPathLength) {
            return true;

            // 回溯
            pos = stack.back()[1] + 1;
            visited.erase(stack.back()[0]);
            stack.pop_back();
        } 
        else {
            vector<int> currentPair = stack.back();
            int currentVertexId = currentPair[0];
            int flag = true;

            if (pos >= g.v[currentVertexId].deg) {
                pos = stack.back()[1] + 1;
                visited.erase(stack.back()[0]);
                stack.pop_back();
                continue;
            }
            
            while (g.v[g.v[currentVertexId].neighbors[pos]].type != metaPath[stack.size()] 
                    || visited.find(g.v[currentVertexId].neighbors[pos]) != visited.end()) {
                ++pos;
                if (pos >= g.v[currentVertexId].deg) {
                    pos = stack.back()[1] + 1;
                    visited.erase(stack.back()[0]);
                    stack.pop_back();
                    flag = false;
                    break;
                }
            }

            if (flag) {
                stack.push_back({g.v[currentVertexId].neighbors[pos], pos});
                visited.insert(g.v[currentVertexId].neighbors[pos]);
                pos = 0;
            }
        }
    }
    return false;
}

void joinedImpact(int delVertexId, int delIndex, set<int>& ImpactSet) {
    // 前半 path1, 包含自身
    vector<VertexType> path1(metaPath.begin(), metaPath.begin() + delIndex + 1);
    reverse(path1.begin(), path1.end());
    // 后半 path2, 包含自身
    vector<VertexType> path2(metaPath.begin() + delIndex, metaPath.end());

    // 找到从 delVertexId 开始的 path1 实例
    vector<vector<int>> instances1;
    bfsFindPathInstances(delVertexId, path1, instances1);

    // 找到从 delVertexId 开始的 path2 实例
    vector<vector<int>> instances2;
    bfsFindPathInstances(delVertexId, path2, instances2);

    // join two instance
    for (const auto& instance1 : instances1) {
        for (const auto& instance2 : instances2) {
            vector<int> joinedPath = instance1;
            reverse(joinedPath.begin(), joinedPath.end());
            joinedPath.insert(joinedPath.end(), instance2.begin() + 1, instance2.end());

            unordered_set<int> seen(joinedPath.begin(), joinedPath.end()); 
            if (seen.size() == joinedPath.size()) {
                for (auto& vertexId : joinedPath) {
                    ImpactSet.insert(vertexId);
                }
            }
        }
    }
}

bool joinedHasInstance(int id, int vertexIndex) {
    // 前半 path1, 包含自身
    vector<VertexType> path1(metaPath.begin(), metaPath.begin() + vertexIndex + 1);
    reverse(path1.begin(), path1.end());
    // 后半 path2, 包含自身
    vector<VertexType> path2(metaPath.begin() + vertexIndex, metaPath.end());

    // 找到从 delVertexId 开始的 path1 实例
    vector<vector<int>> instances1;
    bfsFindPathInstances(id, path1, instances1);

    // 找到从 delVertexId 开始的 path2 实例
    vector<vector<int>> instances2;
    bfsFindPathInstances(id, path2, instances2);

    // join two instance
    for (const auto& instance1 : instances1) {
        for (const auto& instance2 : instances2) {
            vector<int> joinedPath = instance1;
            reverse(joinedPath.begin(), joinedPath.end());
            joinedPath.insert(joinedPath.end(), instance2.begin() + 1, instance2.end());

            unordered_set<int> seen(joinedPath.begin(), joinedPath.end()); 
            if (seen.size() == joinedPath.size()) {
                return true;
            }
        }
    }
    return false;
}

void bfsFindPathInstances(int delVertexId, const vector<VertexType>& path, vector<vector<int>>& instances) {
    queue<vector<int>> q;
    vector<int> currentPath;
    currentPath.push_back(delVertexId);
    q.push(currentPath);
    int pathLength = path.size();

    while (!q.empty()) {
        currentPath = q.front();
        q.pop();
        int currentVertex = currentPath[currentPath.size() - 1];
        if (currentPath.size() == pathLength) {
            instances.push_back(currentPath);
        }
        else {
            for (int i = 0; i < g.v[currentVertex].deg; ++i) {
                int neighbor = g.v[currentVertex].neighbors[i];
                if (g.v[neighbor].type == path[currentPath.size()] 
                        && !isValInVector(neighbor, currentPath)) {
                    currentPath.push_back(neighbor);
                    q.push(currentPath);
                    currentPath.pop_back();
                }
            }
        }
    }
}

void delVertex(int id) {
    auto it = find(H.begin(), H.begin() + Hsize, id);
    if (it != H.begin() + Hsize) {
        swap(*it, H[Hsize - 1]);
        --Hsize;
        if (shrink_stack_size != 0) {
            shrink_stack[shrink_stack_size].delHsize += 1;
            if (g.v[id].type == targetType) {
                shrink_stack[shrink_stack_size].delTargetNum += 1;
            }
        }
    }
    for (int i = 0; i < g.v[id].deg; ++i) {
        int neighbor = g.v[id].neighbors[i];
        auto it = find(g.v[neighbor].neighbors.begin(), g.v[neighbor].neighbors.begin() + g.v[neighbor].deg, id);
        if (it != g.v[neighbor].neighbors.begin() + g.v[neighbor].deg) {
            swap(*it, g.v[neighbor].neighbors[g.v[neighbor].deg - 1]);
            g.v[neighbor].deg -= 1;
            if (shrink_stack_size != 0) {
                shrink_stack[shrink_stack_size].delDegree[neighbor] += 1;
            }
        }
    }
    if (shrink_stack_size != 0) {
        shrink_stack[shrink_stack_size].delDegree[id] += g.v[id].deg;
    }
    g.v[id].deg = 0;
}

bool isValInVector(int val, const vector<int>& vec) {
    int vecLength = vec.size();
    for (int i = 0; i < vecLength; ++i) {
        if (vec[i] == val) {
            return true; 
        }
    }
    return false;
}

void printH() {
    ofstream outfile(OUTPUT_FILE, ios::app);
	if (!outfile.is_open()) {
        std::cerr << "Can not open output file: " << OUTPUT_FILE << std::endl;
        return ;
    }
    int vertexNumber = Hsize;
    int vertexTargetNumber = 0;
    double avgDegree = 0;
    double avgPScore = 0;
    int min_pscore = INT_MAX;
    int max_pscore = INT_MIN;
    for (int i = 0; i < Hsize; ++i) {
        int vertexId = H[i];
        if (g.v[vertexId].type == targetType) {
            vertexTargetNumber += 1;
            int pscore = Psupport(vertexId);
            if (pscore < min_pscore) {
                min_pscore = pscore;
            }
            if (pscore > max_pscore) {
                max_pscore = pscore;
            }
            avgPScore += pscore;
        }
        avgDegree += g.v[vertexId].deg;
    }
    avgDegree = avgDegree / vertexNumber;
    avgPScore = avgPScore / vertexTargetNumber;
    outfile << fixed << setprecision(4) << 
        "There is " << vertexNumber << " vertex in H" << endl <<
        "There is " << vertexTargetNumber << " target type vertex in H" << endl <<
        "The avgDegree is: "<< avgDegree << endl <<
        "The avgPScore is: "<< avgPScore << endl <<
        "The minPScore is: "<< min_pscore << endl <<
        "The maxPScore is: "<< max_pscore << endl;
    outfile.close();
}

void printParameter(){
    ofstream outfile(OUTPUT_FILE, ios::app);
	if (!outfile.is_open()) {
        std::cerr << "Can not open output file: " << OUTPUT_FILE << std::endl;
        return ;
    }
    outfile << endl << "File: " << HIN_PATH << endl;
    outfile << "queryVertexId: " << queryVertexId << endl;
    outfile << "metaPath: " ;
    for (const auto& type : metaPath) {
        outfile << type << " ";
    }
    outfile << endl;
    outfile << "degreeThreshold: " << endl;;
    for (const auto& pair : degreeThreshold) {
        outfile << "  " << pair.first << ": " << pair.second << endl;
    }
    outfile << "k: " << k << endl;
    outfile << "Size Constraint: " << size_constraint << endl;
    outfile << "dpcore_time: " << dpcore_time.count() << " ms" << endl;
    duration_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    outfile << "time: " << duration_time.count() << " ms" << endl;
    outfile << "back track: " << back_track_times << endl;
    outfile << "iteration: " << iteration_times << endl;
    outfile.close();
}

void updateHConnect() {
    // TODO: change to vector
    vector<int> tempH;
    unordered_map<int, bool> visited;
    vector<int> stack;
    stack.push_back(queryVertexId);
    visited[queryVertexId] = true;
    while(!stack.empty()) {
        int vertexId = stack.back();
        stack.pop_back();
        tempH.push_back(vertexId);
        for (int i = 0; i < g.v[vertexId].deg; ++i) {
            int neighbor = g.v[vertexId].neighbors[i];
            if (!visited[neighbor]) {
                stack.push_back(neighbor);
                visited[neighbor] = true;
            }
        }
    }
    if (Hsize != tempH.size()) {
        H = tempH;
        Hsize = tempH.size();
    }
}

void constructMetaGraphfrom(int id) {
    list<vector<int>> stack;
    unordered_map<int, int> targetVertexCount;
    unordered_set<int> visited;
    // {vertexId, last vertex's neighbor number}
    stack.push_back({id, 0});
    visited.insert(id);
    int pos = 0;

    while (!stack.empty()) {
        if (stack.size() == metaPathLength) {
            targetVertexCount[stack.back()[0]]++;
            for (auto& vec : stack) {
                // Note: 目前都按非对称算的，instanceNum会是正常的2倍，
                //       由于对称路径中123和321被视为不同的metapath instance，但不影响运算正确性
                // Attr: instanceNum
                g.v[vec[0]].instanceNum++;
            }
            pos = stack.back()[1] + 1;
            visited.erase(stack.back()[0]);
            stack.pop_back();
        } 
        else {
            vector<int> currentPair = stack.back();
            int currentVertexId = currentPair[0];
            int flag = true;
            if (pos >= g.v[currentVertexId].deg) {
                pos = stack.back()[1] + 1;
                visited.erase(stack.back()[0]);
                stack.pop_back();
                continue;
            }
            while (g.v[g.v[currentVertexId].neighbors[pos]].type != metaPath[stack.size()] 
                    || visited.find(g.v[currentVertexId].neighbors[pos]) != visited.end()) {
                ++pos;
                if (pos >= g.v[currentVertexId].deg) {
                    pos = stack.back()[1] + 1;
                    visited.erase(stack.back()[0]);
                    stack.pop_back();
                    flag = false;
                    break;
                }
            }
            if (flag) {
                stack.push_back({g.v[currentVertexId].neighbors[pos], pos});
                visited.insert(g.v[currentVertexId].neighbors[pos]);
                pos = 0;
            }
        }
    }

    int Psupport = 0;
    // metaGraph
    for (const auto& vertexCountPair : targetVertexCount) {
        if (vertexCountPair.second >= Psupport) {
            metaGraph[id][vertexCountPair.first] = vertexCountPair.second;
        }
    }
    return ;
}

void delVertexAndUpdateQ(int id, vector<int>& Q, unordered_set<int>& isInQ) {
    // find all path instances pass 'id' vertex
    // for each path:
    //      update metaGraph -> if two target vertex's P-support() < k -> Q.insert()(need to consider repeat)
    //      update each vertex's instanceNum -> if instanceNum == 0 -> Q.push_back()(will not repeat)
    for (int i = 0; i < g.v[id].deg; i++) {
        int neighbor = g.v[id].neighbors[i];
        if (g.v[neighbor].deg == degreeThreshold[g.v[neighbor].type]) {
            Q.push_back(neighbor);
        }
    }
    VertexType delVertexType = g.v[id].type;
    vector<int> delVertexPos;
    for (int i = 0; i < metaPathLength; ++i) {
        if (metaPath[i] == delVertexType) {
            delVertexPos.push_back(i);
        }
    }
    for (const auto& delIndex : delVertexPos) {
        // DFS find instances the vertex in
        if (delIndex == 0 || delIndex == metaPathLength - 1) {
            vector<VertexType> path;
            if (delIndex == metaPathLength-1) {
                vector<VertexType> metaPathReverse(metaPath.begin(), metaPath.end());
                reverse(metaPathReverse.begin(), metaPathReverse.end());
                path = metaPathReverse;
            }
            else {
                path = metaPath;
            }
            list<vector<int>> stack;
            size_t metaPathLength = metaPath.size();
            unordered_set<int> visited;
            
            // {vertexId, pos}
            stack.push_back({id, 0});
            visited.insert(id);
            int pos = 0;

            
            while (!stack.empty()) {
                if (stack.size() == metaPathLength) {
                    // updateMetaGraph
                    if (delIndex == metaPathLength-1) {
                        metaGraph[stack.back()[0]][stack.front()[0]] -= 1;
                        if (shrink_stack_size != 0) {
                            shrink_stack[shrink_stack_size].delMetaGraph[stack.back()[0]][stack.front()[0]] += 1;
                        }
                        if (Psupport(stack.back()[0]) < k && isInQ.find(stack.back()[0]) == isInQ.end() && stack.back()[0] != id) {
                            Q.push_back(stack.back()[0]);
                        }
                    }
                    else {
                        metaGraph[stack.front()[0]][stack.back()[0]] -= 1;
                        if (shrink_stack_size != 0) {
                            shrink_stack[shrink_stack_size].delMetaGraph[stack.front()[0]][stack.back()[0]] += 1;
                        }
                        if (Psupport(stack.front()[0]) < k && isInQ.find(stack.front()[0]) == isInQ.end() && stack.front()[0] != id) {
                            Q.push_back(stack.front()[0]);
                        }
                    }
                    // updateInstanceNum
                    for (auto& pair : stack) {
                        g.v[pair[0]].instanceNum -= 1;
                        if (shrink_stack_size != 0) {
                            shrink_stack[shrink_stack_size].delInstanceNum[pair[0]] += 1;
                        }
                        if (g.v[pair[0]].instanceNum == 0 && isInQ.find(pair[0]) == isInQ.end() && pair[0] != id) {
                            Q.push_back(pair[0]);
                        }
                    }

                    // 回溯
                    pos = stack.back()[1] + 1;
                    visited.erase(stack.back()[0]);
                    stack.pop_back();
                } 
                else {
                    vector<int> currentPair = stack.back();
                    int currentVertexId = currentPair[0];
                    int flag = true;

                    if (pos >= g.v[currentVertexId].deg) {
                        pos = stack.back()[1] + 1;
                        visited.erase(stack.back()[0]);
                        stack.pop_back();
                        continue;
                    }
                    
                    while (g.v[g.v[currentVertexId].neighbors[pos]].type != metaPath[stack.size()] 
                            || visited.find(g.v[currentVertexId].neighbors[pos]) != visited.end()) {
                        ++pos;
                        if (pos >= g.v[currentVertexId].deg) {
                            pos = stack.back()[1] + 1;
                            visited.erase(stack.back()[0]);
                            stack.pop_back();
                            flag = false;
                            break;
                        }
                    }

                    if (flag) {
                        stack.push_back({g.v[currentVertexId].neighbors[pos], pos});
                        visited.insert(g.v[currentVertexId].neighbors[pos]);
                        pos = 0;
                    }
                }
            }
        }
        // Joined find instance the vertex in
        else {
            vector<vector<int>> joinedInstances;
            // 前半 path1, 包含自身
            vector<VertexType> path1(metaPath.begin(), metaPath.begin() + delIndex + 1);
            reverse(path1.begin(), path1.end());
            // 后半 path2, 包含自身
            vector<VertexType> path2(metaPath.begin() + delIndex, metaPath.end());

            // 找到从 delVertexId 开始的 path1 实例
            vector<vector<int>> instances1;
            bfsFindPathInstances(id, path1, instances1);

            // 找到从 delVertexId 开始的 path2 实例
            vector<vector<int>> instances2;
            bfsFindPathInstances(id, path2, instances2);

            // join two instance
            for (const auto& instance1 : instances1) {
                for (const auto& instance2 : instances2) {
                    int sourceVertexId = instance1[path1.size()-1];
                    int targetVertexId = instance2[path2.size()-1];
                    unordered_set<int> seen(instance1.begin(), instance1.end());
                    for (int i : instance2) {
                        seen.insert(i);
                    }
                    if (seen.size() == metaPathLength) {
                        // update metaGraph
                        metaGraph[sourceVertexId][targetVertexId] -= 1;
                        if (shrink_stack_size != 0) {
                            shrink_stack[shrink_stack_size].delMetaGraph[sourceVertexId][targetVertexId] += 1;
                        }
                        if (Psupport(sourceVertexId) < k && isInQ.find(sourceVertexId) == isInQ.end() && sourceVertexId != id) {
                            Q.push_back(sourceVertexId);
                        }
                        // update instanceNum
                        for (const auto& i : seen) {
                            g.v[i].instanceNum -= 1;
                            if (shrink_stack_size != 0) {
                                shrink_stack[shrink_stack_size].delInstanceNum[i] += 1;
                            }
                            if (g.v[i].instanceNum == 0 && isInQ.find(i) == isInQ.end() && i != id) {
                                Q.push_back(i);
                            }
                        }
                    }
                }
            }
        }
    }
}

void basic_shrink() {
    target_vertex_number = 0;
    for (int i = 0; i < Hsize; ++i) {
        if (g.v[H[i]].type == targetType) {
            target_vertex_number++;
        }
    }
    if (target_vertex_number <= size_constraint) {
        return ;
    }
    shrink_stack.resize(target_vertex_number - size_constraint + 1);
    shrink_stack[shrink_stack_size].Candidate = {queryVertexId};
    shrink_stack[shrink_stack_size].isInCandidate = {queryVertexId};


    while (true) {
        iteration_times++;
        // cout << "target_vertex_number: " << target_vertex_number << endl;
        // if promising branch -> delete
        // else back track
        // 随机选，就选id最小的
        int i = 0;
        while (g.v[H[i]].type != targetType || shrink_stack[shrink_stack_size].isInCandidate.find(H[i]) != shrink_stack[shrink_stack_size].isInCandidate.end()) {
            i++;
        }
        // 如果当前所有Candidate都在H中，且，有可选择的节点
        if (shrink_stack[shrink_stack_size].Candidate.size() <= size_constraint && isPromingBranch() && i < Hsize) {
            // cout << "select " << H[i] << " to delete when shrink_stack_size: " << shrink_stack_size << endl;
            int selected_id = H[i];
            // 删除v，并将影响记录在stack中
            shrink_stack_size++;
            shrink_stack[shrink_stack_size].delTargetNum = 0;
            shrink_stack[shrink_stack_size].delHsize = 0;
            shrink_stack[shrink_stack_size].selectedVertexId = selected_id;
            shrink_stack[shrink_stack_size].Candidate = shrink_stack[shrink_stack_size-1].Candidate;
            shrink_stack[shrink_stack_size].isInCandidate = shrink_stack[shrink_stack_size-1].isInCandidate;
            vector<int> Q = {selected_id};
            unordered_set<int> isInQ = {selected_id};
            while (!Q.empty()) {
                int id = Q.back();
                Q.pop_back();
                isInQ.erase(id);
                for (int i = 0; i < g.v[id].deg; i++) {
                    int neighbor = g.v[id].neighbors[i];
                    if (g.v[neighbor].deg == degreeThreshold[g.v[neighbor].type]) {
                        Q.push_back(neighbor);
                        isInQ.insert(neighbor);
                    }
                }
                // 在这个函数里修改metaGraph和instanceNum
                delVertexAndUpdateQ(id, Q, isInQ);
                // 在这个函数里修改deg和neighbor
                delVertex(id);
            }
            // updateHConnect();
            target_vertex_number -= shrink_stack[shrink_stack_size].delTargetNum;
            // cout << "After shrink, target_vertex_number: " << target_vertex_number << endl;
        }
        // 回溯
        else {
            back_track_times++;
            // std:: cout << "back track when shrink_stack_size: " << shrink_stack_size << std::endl;
            if (shrink_stack_size == 0) {
                Hsize = 0;
                return ;
            }
            // recover Hsize
            Hsize += shrink_stack[shrink_stack_size].delHsize;
            // recover TargetNum
            target_vertex_number += shrink_stack[shrink_stack_size].delTargetNum;
            shrink_stack[shrink_stack_size].delTargetNum = 0;
            // recover degree
            for (auto pair : shrink_stack[shrink_stack_size].delDegree) {
                g.v[pair.first].deg += pair.second;
            }
            shrink_stack[shrink_stack_size].delDegree.clear();
            // recover metaGraph
            for (auto outer_pair : shrink_stack[shrink_stack_size].delMetaGraph) {
                int source_vertex = outer_pair.first;
                auto inner_map = outer_pair.second;
                for (auto inner_pair : inner_map) {
                    int target_vertex = inner_pair.first;
                    metaGraph[source_vertex][target_vertex] += inner_pair.second;
                }
            }
            shrink_stack[shrink_stack_size].delMetaGraph.clear();
            // recover instanceNum
            for (auto pair : shrink_stack[shrink_stack_size].delInstanceNum) {
                g.v[pair.first].instanceNum += pair.second;
            }
            shrink_stack[shrink_stack_size].delInstanceNum.clear();
            // 更新下一轮迭代所需的CandidateSet
            shrink_stack[shrink_stack_size-1].Candidate.push_back(shrink_stack[shrink_stack_size].selectedVertexId);
            shrink_stack[shrink_stack_size-1].isInCandidate.insert(shrink_stack[shrink_stack_size].selectedVertexId);
            shrink_stack[shrink_stack_size] = shrink_stack_object();
            --shrink_stack_size;
        }
        // for (int i = 0; i < Hsize; ++i) {
        //     cout << H[i] << "-> deg: " << g.v[H[i]].deg << "; instanceNum: " << g.v[H[i]].instanceNum << "; neighbor: ";
        //     for (auto it : g.v[H[i]].neighbors) {
        //         cout << it << " ";
        //     }
        //     cout << endl;
        // }
        // cout << "Candidate: ";
        // for (auto it : shrink_stack[shrink_stack_size].Candidate) {
        //     cout << it << " ";
        // }
        // cout << endl;
        if (Hsize != 0 && target_vertex_number <= size_constraint && isPromingBranch()) {
            break;
        }
    }
    // cout << "while loop over, Hsize: " << Hsize << endl;
    return ;
}

// determine whether current branch Set(CandidateSet) - set(H) == 0
bool isPromingBranch() {
    int count = 0;
    for (int i = 0; i < Hsize; i++) {
        if (shrink_stack[shrink_stack_size].isInCandidate.find(H[i]) != shrink_stack[shrink_stack_size].isInCandidate.end()) {
            count++;
        }
    }
    bool res = false;
    if (count == shrink_stack[shrink_stack_size].Candidate.size()) {
        res =  true;
    }
    return res;
}


void DPcore_lb() {
    vector<int> Q;
    H.resize(g.n);
    // 去掉与q不连通的节点
    updateHConnect();
    for (int i = 0; i < Hsize; i++)
    {
        if (g.v[H[i]].deg < degreeThreshold[g.v[H[i]].type]) {
            Q.push_back(H[i]);
        }
    }
    int iteration = 0;
    while (!Q.empty())
    {
        iteration++;
        int id = Q.back();
        Q.pop_back();
        for (int i = 0; i < g.v[id].deg; i++) {
            int neighbor = g.v[id].neighbors[i];
            if (g.v[neighbor].deg == degreeThreshold[g.v[neighbor].type]) {
                Q.push_back(neighbor);
            }
        }
        delVertex(id);
    }

    for (int i = 0; i < Hsize; i++) {
        if (g.v[H[i]].type == targetType) {
            int sup = estimate_Psupport(H[i]);
            if (sup < k) {
                Q.push_back(H[i]);
            }
        }
    }
    
    while (!Q.empty())
    {
        iteration++;
        int id = Q.back();
        Q.pop_back();
        for (int i = 0; i < g.v[id].deg; i++) {
            int neighbor = g.v[id].neighbors[i];
            if (g.v[neighbor].deg == degreeThreshold[g.v[neighbor].type]) {
                Q.push_back(neighbor);
            }
        }
        delVertex(id);
    }
    
    unordered_set<int> isInQ;
    for (int i = 0; i < Hsize; i++) {
        if (g.v[H[i]].type == targetType) {
            constructMetaGraphfrom(H[i]);
            if (Psupport(H[i]) < k) {
                Q.push_back(H[i]);
                isInQ.insert(H[i]);
            }
        }
    }
    for (int i = 0; i < Hsize; i++) {
        if (g.v[H[i]].type != targetType && g.v[H[i]].instanceNum == 0) {
            Q.push_back(H[i]);
            isInQ.insert(H[i]);
        }
    }

    while (!Q.empty()) {
        int id = Q.back();
        Q.pop_back();
        isInQ.erase(id);
        for (int i = 0; i < g.v[id].deg; i++) {
            int neighbor = g.v[id].neighbors[i];
            if (g.v[neighbor].deg == degreeThreshold[g.v[neighbor].type]) {
                Q.push_back(neighbor);
                isInQ.insert(neighbor);
            }
        }
        delVertexAndUpdateQ(id, Q, isInQ);
        delVertex(id);
    }

    updateHConnect();
    return ;
}

int estimate_Psupport(int id) {
    unordered_map<int, int> x;
    x[id] = 1;
    for (int i = 1; i < metaPathLength; i++) {
        unordered_map<int, int> temp_x;
        for (auto pair : x) {
            for (int j = 0; j < g.v[pair.first].deg; j++) {
                int neighbor = g.v[pair.first].neighbors[j];
                if (g.v[neighbor].type == metaPath[i]) {
                    temp_x[neighbor] += pair.second;
                }
            }
        }
        x = temp_x;        
    }
    int sup = 0;
    for(auto pair : x) {
        if (pair.second > sup && pair.first != id) {
            sup = pair.second;
        }
    }
    return sup;
}
