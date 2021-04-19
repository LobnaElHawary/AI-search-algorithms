#include <iostream>
#include <queue>
#include <vector>
#include <fstream>
#include <string>
#include <stack>
#include <cmath>
#include <algorithm>
using namespace std;

//structs
struct visited
{
    string city;
    // 0 if not, 1 if visited
    bool visited_flag;
};

struct node
{
    string city;
    int cost;
    vector<node *>child;
    
    //to be able to go back and retrieve path
    string parent_city;
    node* parent;
    int height;
    int heuristic;
};

//creating new node fn
node *newNode(string city, int cost, string parent_city, node* parent, int height, int heuristc)
{
    node *temp = new node;
    temp->cost = cost;
    temp->city = city;
    temp->parent_city = parent_city;
    temp->parent = parent;
    temp->height = height;
    temp->heuristic = heuristc;
    return temp;
}

// finds the number of unique cities in a file
int unique_cities(string file_name)
{
    string line, substr, city_1;
    std::vector<string> v;
    
    ifstream myfile(file_name);
    unsigned long position_1, position_2;
    if (myfile.is_open())
    {
        while (getline (myfile,line))
        {
            position_1 = line.find_first_of(" ");
            position_2 = line.find_last_of(" ");
            
            //city1 name
            city_1 = line.substr(1,position_1 - 1);
            v.push_back(city_1);
            
            substr= line.substr(position_1 + 1, (position_2 - position_1) - 1);
            v.push_back(substr);
            //city 2 name
        }
    }
    std::sort(v.begin(), v.end());
    int uniqueCount = std::unique(v.begin(), v.end()) - v.begin();
    
    return uniqueCount;
}

//fill up array with city names and initialize to 0
void initialize_array(string file_name, visited visited_array[], int cities)
{
    
    string line, city_1, substr;
    //positions of 1st and 2ns space in line
    unsigned long position_1, position_2;
    //number of found city_1 and city_2
    int found_2, found_1;
    //index
    int j;
    int city_count;
    ifstream myfile(file_name);
    
    //initialization
    j = 0;
    city_count = 0;
    
    if (myfile.is_open())
    {
        
        while (getline (myfile,line))
        {
            
            
            position_1 = line.find_first_of(" ");
            position_2 = line.find_last_of(" ");
            
            city_1 = line.substr(1,position_1 - 1);
            //city_2
            substr= line.substr(position_1 + 1, (position_2 - position_1) - 1);
            // search array, and if city_1 and substr dont exist, add them
            found_1 = 0;
            found_2 = 0;
            for(int i=0; i< cities; i++)
            {
                if(visited_array[i].city == city_1)
                {
                    found_1++;
                }
                
                if(visited_array[i].city == substr)
                {
                    found_2++;
                }
            }
            
            if(found_1 == 0)
            {
                visited_array[j] = {city_1, 0};
                j++;
                city_count++;
            }
            if(found_2 == 0)
            {
                visited_array[j] = {substr, 0};
                j++;
                city_count++;
            }
            
        }
    }
    
    myfile.close();
    
}
//prints out results and finds path
void results(node* p, int nodes_visited)
{
    stack<node*> final_path;
    int final_path_cost = 0;
    
    cout<<endl;
    cout<<"----------------- PATH ----------------------"<<endl;
    cout<<endl;
    cout<<"the path is "<<endl;
    //end city
    final_path.push(p);
    final_path_cost += p->cost;
    
    //if has node has a parent
    while(p->parent != NULL)
    {
        final_path.push(p->parent);
        final_path_cost += p->parent->cost;
        p = p->parent;
    }
    
    
    //print path
    for(unsigned long i=final_path.size(); i>0; i--)
    {
        cout<<final_path.top() -> city<< " ";
        final_path.pop();
    }
    cout<<"and it costs "<<final_path_cost<<endl;
    cout<<endl;
    cout<<"----------------- VISITED -------------------"<<endl;
    cout<<endl;
    cout<<"The number of visited nodes are "<<nodes_visited<<endl;
    cout<<endl;
    cout<<"----------------------------------------------"<<endl;
}

//function that calculates heuristic
double heuristic(string file_name, string city_1, string city_2, int factor)
{
    //declarations
    ifstream pos(file_name);
    string line, substr;
    bool found_1 = false; //indicates if city_1 is found
    bool found_2 = false; //indicates if city_2 is found
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    size_t dot_index; //positions of the "."
    size_t index_1, index_2; // indeces of city1 and city_2
    
    while ( !(found_1 && found_2) && getline(pos, line))
    {
        
        index_1 = line.find(city_1);
        index_2 = line.find(city_2);
        
        //if city_1 is found
        if(index_1!=std::string::npos)
        {
            found_1 = true;
            
            dot_index = line.find_first_of(".");
        
            substr = line.substr(dot_index - 4, 4);
            x1 = stoi(substr);
            
            //retrieving the y-axis for city 1
            substr = line.substr(dot_index + 1);
            y1 = stoi(substr);
        }
        
        //if city_2 is found
        if(index_2!=std::string::npos)
        {
            found_2 = true;
            
            //finding the . position
            dot_index = line.find_first_of(".");
            
            //retrieving the x-axis for city 2
            substr = line.substr(dot_index - 4, 4);
            x2 = stoi(substr);
            
            //retrieving the y-axis for city 2
            substr = line.substr(dot_index + 1);
            y2 = stoi(substr);
        }
    }
    
    return (factor * sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2)));
}

//-------------------------- BFS -----------------------------//
void BFS(string file_name, node *root, string end_city)
{
    
    int cities; //find number of cities in file
    cities = unique_cities(file_name);
    visited *visited_array = new visited[cities]; //dynamic arry of struct visited
    string line, cost_str, city_1, city_2;
    size_t position_1, position_2; //position of first and second spaces " "
    int cost, i, nodes_visited;
    queue<node *> q;  // Create a queue
    
    //initialization
    nodes_visited = 0;
    
    initialize_array(file_name,visited_array,cities);  //fill up array with city names and initialize to 0
    
    q.push(root); // enqueue root
    
    //find root in visited table and mark it visited
    for(int i=0;i<cities;i++)
    {
        if(root->city == visited_array[i].city)
        {
            visited_array[i].visited_flag = 1;
            
        }
    }
    
    
    while (!q.empty())
    {
        unsigned long n = q.size();
        
        while (n > 0)
        {
            //print item in queue
            node * p = q.front();
            
            q.pop();
            nodes_visited++; // if a node has been popped, it was visited
            
            //if goal city is reached
            if(p->city == end_city)
            {
                results(p,nodes_visited);
                return;
            }
            
            
            //generate children on what is in queue
            ifstream myfile(file_name);
            if (myfile.is_open())
            {
                while (getline (myfile,line))
                {
                    position_1 = line.find_first_of(" ");
                    position_2 = line.find_last_of(" ");
                    
                    city_1 = line.substr(1,position_1 - 1); //city_1's name
                    city_2 = line.substr(position_1 + 1, (position_2 - position_1) - 1); //city_2's name
                    //find lines with cities same as p-> city
                    i = 0;
                    
                    if(city_1 == p->city)
                    {
                        
                        //cost
                        cost_str = line.substr(position_2+1, line.length() - 1);
                        cost = stoi(cost_str);
                        //generate the children
                        (p->child).push_back(newNode(city_2,cost,p->city, p, p->height + 1, 0));
                    }
                    
                }
                myfile.close();
            }
            
            //enqueue all children of the dequeued item
            for (int i=0; i<p->child.size(); i++)
            {
                for(int j = 0; j<cities;j++)
                {
                    if(visited_array[j].city == p->child[i]->city)
                    {
                        //if not visited, enqueue
                        if(visited_array[j].visited_flag == 0)
                        {
                            visited_array[j].visited_flag = 1;
                            q.push(p->child[i]);
                        }
                    }
                }
                
            }
            n--;
        }
        
        cout << endl;
    }
    
}

//-------------------------- UCS, Greed, Astar -----------------------------//

//read file function to make the tree
void UCS_Greedy_AStar(string file_name, string file_name_pos, node *root, string end_city, int algorithm)
{

    //declarations
    int cities, factor, cost, i, nodes_visited, index, total_cost;
    cities = unique_cities(file_name);
    visited *visited_array = new visited[cities]; //dynamic array representing cities that were already entered in queue
    double heuristic_cost;
    string line, cost_str, city_1, city_2;
    //position of first and second spaces " "
    size_t position_1, position_2;
    unsigned long array_size;
    queue<node *> q;  // Create a queue
    ifstream myfile(file_name);
    
    //initializing
    nodes_visited = 0;
    factor = 1;
    total_cost = 0;
    heuristic_cost = 0;
    
    if((algorithm == 4)||(algorithm == 6))
    {
        cout<<"Please enter the heuristic's factor"<<endl;
        cin>>factor;
    }
    
    initialize_array(file_name,visited_array,cities); //fill up array with city names and initialize to 0
    
    q.push(root); // enqueue root
    
    //find root in visited table and mark it visited
    for(int i=0;i<cities;i++)
    {
        if(root->city == visited_array[i].city)
        {
            visited_array[i].visited_flag = 1;
        }
    }
    
    while (!q.empty())
    {
        unsigned long n = q.size();
        
        while (n > 0)
        {
            //print item in queue
            node * p = q.front();
            q.pop();
            nodes_visited++; // if a node has been popped, it was visited
        
            if(p->city == end_city)
            {
                stack<node*> final_path;
                
        
                    cout<<endl;
                    cout<<"----------------- PATH ----------------------"<<endl;
                    cout<<endl;
                    cout<<"The path costs "<<p->cost<<" ";
                    cout<<"and is "<<endl;
                    //end city
                    final_path.push(p);
                    
                    
                    //if has node has a parent
                    while(p->parent != NULL)
                    {
                        final_path.push(p->parent);
                        p = p->parent;
                    }
                    
                    
                    //print path
                    for(unsigned long i=final_path.size(); i>0; i--)
                    {
                        cout<<final_path.top() -> city<< " ";
                        final_path.pop();
                    }
                    
                    cout<<endl;
                    cout<<endl<<"----------------- VISITED -------------------"<<endl;
                    cout<<endl;
                    cout<<"The number of visited nodes are "<<nodes_visited<<endl;
                    cout<<endl;
                    cout<<"----------------------------------------------"<<endl;
                    return;
            }
            
            //generate children on what is in queue
            ifstream myfile(file_name);
            if (myfile.is_open())
            {
                while (getline (myfile,line))
                {
                    position_1 = line.find_first_of(" ");
                    position_2 = line.find_last_of(" ");
                    
                    //city1 name
                    city_1 = line.substr(1,position_1 - 1);
                    //find lines with cities same as p-> city
                    i = 0;
                    
                    if(city_1 == p->city)
                    {
                        //city 2
                        city_2 = line.substr(position_1 + 1, (position_2 - position_1) - 1);
                        //cost
                        cost_str = line.substr(position_2+1, line.length() - 1);
                        cost = stoi(cost_str);
                        
                        
                        //uniform cost search
                        if(algorithm == 5)
                        {
                            total_cost = cost + p->cost;
                        }
                        //greedy
                        if(algorithm == 4)
                        {
                            heuristic_cost = heuristic(file_name_pos, city_2, end_city, factor);
                            total_cost = cost + p->cost;
                        }
                        //AStar
                        if(algorithm == 6)
                        {
                            heuristic_cost = heuristic(file_name_pos, city_2, end_city, factor);
                            total_cost = cost + p->cost;
                        }
                        //generate the children
                        //cost of child = cost of parent + cost of parent to child
                        (p->child).push_back(newNode(city_2, total_cost, p->city, p, p->height + 1, heuristic_cost));
                    }
                }
                myfile.close();
            }
            
            //push children into queue
            for (int i=0; i<p->child.size(); i++)
            {
                for(int j = 0; j<cities;j++)
                {
                    if(visited_array[j].city == p->child[i]->city)
                    {
                        //if not visited, enqueue
                        if(visited_array[j].visited_flag == 0)
                        {
                            visited_array[j].visited_flag = 1;
                            q.push(p->child[i]);
                        }
                    }
                }
                
            }
            
            array_size = q.size();
            
            node* *array = new node*[array_size]; //array used to sort queue
            index = 0;
            
            //array contains queue elements
            while(!q.empty())
            {
                array[index] = q.front();
                q.pop();
                index ++;
            }
            
            node* temp;
            
            // if UCS, sort by node's cost from file
            if(algorithm == 5)
            {
                //sort array
                for(int i = 1; i <= array_size; i++)
                {
                    for(int j = 0; j < array_size - 1; j++)
                    {
                        
                        if(array[j+1]->cost < array[j]->cost)
                        {
                            
                            //swap array[i] and array[min_index]
                            temp = array[j];
                            array[j] = array[j+1];
                            array[j+1] = temp;
                        }
                        
                    }
                }
            }
            
            // if Greedy, sort by node's cost from file
            if(algorithm == 4)
            {

                //sort array
                for(int i = 1; i <= array_size; i++)
                {
                    for(int j = 0; j < array_size - 1; j++)
                    {
                        
                        if(array[j+1]->heuristic < array[j]->heuristic)
                        {
                            
                            //swap array[i] and array[min_index]
                            temp = array[j];
                            array[j] = array[j+1];
                            array[j+1] = temp;
                        }
                        
                    }
                }
                
            }
            
            //if A* search, sort using heuristic + cost
            if(algorithm == 6)
            {
            
                //sort array
                for(int i = 1; i <= array_size; i++)
                {
                    for(int j = 0; j < array_size - 1; j++)
                    {
                        
                        if((array[j+1]->heuristic)+ (array[j+1]->cost) < (array[j]->heuristic + array[j]->cost))
                        {
                            
                            //swap array[i] and array[min_index]
                            temp = array[j];
                            array[j] = array[j+1];
                            array[j+1] = temp;
                        }
                        
                    }
                }
            }
            //push sorted array elements into queue
            
            for(int i=0;i<array_size;i++)
            {
                q.push(array[i]);
            }

            n--;
        }
        
        cout << endl;
    }
}
//-------------------------- DFS -----------------------------//
void DFS(string file_name, node *root, string end_city)
{
    int cities; //find number of cities in file
    cities = unique_cities(file_name);
    visited *visited_array = new visited[cities]; //dynamic arry of struct visited
    string line, cost_str, city_1, city_2;
    size_t position_1, position_2; //position of first and second spaces " "
    int cost, i, nodes_visited;
    stack<node *> s;  // Create a stack
    
    //initialization
    nodes_visited = 0;
    
    initialize_array(file_name,visited_array,cities);  //fill up array with city names and initialize to 0
    
    s.push(root); // enqueue root
    
    //find root in visited table and mark it visited
    for(int i=0;i<cities;i++)
    {
        if(root->city == visited_array[i].city)
        {
            visited_array[i].visited_flag = 1;
            
        }
    }
    
    
    while (!s.empty())
    {
        unsigned long n = s.size();
        
        while (n > 0)
        {
            //print item in queue
            node * p = s.top();
            
            s.pop();
            nodes_visited++; // if a node has been popped, it was visited
            //if goal city is reached
            if(p->city == end_city)
            {
                results(p,nodes_visited);
                return;
            }
            
            
            //generate children on what is in queue
            ifstream myfile(file_name);
            if (myfile.is_open())
            {
                while (getline (myfile,line))
                {
                    position_1 = line.find_first_of(" ");
                    position_2 = line.find_last_of(" ");
                    
                    city_1 = line.substr(1,position_1 - 1); //city_1's name
                    city_2 = line.substr(position_1 + 1, (position_2 - position_1) - 1); //city_2's name
                    //find lines with cities same as p-> city
                    i = 0;
                    
                    if(city_1 == p->city)
                    {
                        
                        //cost
                        cost_str = line.substr(position_2+1, line.length() - 1);
                        cost = stoi(cost_str);
                        //generate the children
                        (p->child).push_back(newNode(city_2,cost,p->city, p, p->height + 1, 0));
                    }
                    
                }
                myfile.close();
            }
            
            //enqueue all children of the dequeued item
            for (int i=0; i<p->child.size(); i++)
            {
                for(int j = 0; j<cities;j++)
                {
                    if(visited_array[j].city == p->child[i]->city)
                    {
                        //if not visited, enqueue
                        if(visited_array[j].visited_flag == 0)
                        {
                            visited_array[j].visited_flag = 1;
                            s.push(p->child[i]);
                        }
                    }
                }
                
            }
            n--;
        }
        
        cout << endl;
    }
}

void DLS(string file_name, node *root, string end_city, int stopping_height)
{
    int cities; //find number of cities in file
    cities = unique_cities(file_name);
    visited *visited_array = new visited[cities]; //dynamic arry of struct visited
    string line, cost_str, city_1, city_2;
    size_t position_1, position_2; //position of first and second spaces " "
    int cost, i, nodes_visited;
    stack<node *> s;  // Create a stack
    
    //initialization
    nodes_visited = 0;
    
    initialize_array(file_name,visited_array,cities);  //fill up array with city names and initialize to 0
    
    s.push(root); // enqueue root
    
    //find root in visited table and mark it visited
    for(int i=0;i<cities;i++)
    {
        if(root->city == visited_array[i].city)
        {
            visited_array[i].visited_flag = 1;
            
        }
    }
    
    while (!s.empty())
    {
        unsigned long n = s.size();
        
        while (n > 0)
        {
            //print item in queue
            node * p = s.top();
            
            s.pop();
            nodes_visited++; // if a node has been popped, it was visited
            //if goal city is reached
            if(p->city == end_city)
            {
                results(p,nodes_visited);
                return;
            }
            
            
            //generate children on what is in queue
            ifstream myfile(file_name);
            if (myfile.is_open())
            {
                while (getline (myfile,line))
                {
                    position_1 = line.find_first_of(" ");
                    position_2 = line.find_last_of(" ");
                    
                    city_1 = line.substr(1,position_1 - 1); //city_1's name
                    city_2 = line.substr(position_1 + 1, (position_2 - position_1) - 1); //city_2's name
                    //find lines with cities same as p-> city
                    i = 0;
                    
                    if(city_1 == p->city)
                    {
                        
                        //cost
                        cost_str = line.substr(position_2+1, line.length() - 1);
                        cost = stoi(cost_str);
                        //generate the children
                        if(p->height < stopping_height)
                        {
                            (p->child).push_back(newNode(city_2,cost,p->city, p, p->height + 1, 0));
                        }
                    }
                    
                }
                myfile.close();
            }
            
            //enqueue all children of the dequeued item
            for (int i=0; i<p->child.size(); i++)
            {
                for(int j = 0; j<cities;j++)
                {
                    if(visited_array[j].city == p->child[i]->city)
                    {
                        //if not visited, enqueue
                        if(visited_array[j].visited_flag == 0)
                        {
                            visited_array[j].visited_flag = 1;
                            s.push(p->child[i]);
                        }
                    }
                }
                
            }
            n--;
        }
        
        cout << endl;
    }
}


//-------------------------- IDS -----------------------------//
void IDS(string file_name, node *root, string end_city, int limit)
{
    for (int i = 0; i <= limit; i++)
    {
        DLS(file_name,root,end_city, i);
    }

}

// Driver program
int main()
{
    
    //declaration
    string start_city, end_city, search,file_name_pos, file_name_road, file_name_temp;
    int algorithm, limit;
    
    // user prompt
    do
    {
        cout<<" Please enter the number refering to the algorithm used"<<endl;
        cout<<"-------------------------------------------------------"<<endl;
        cout<<" 1: BFS "<<endl;
        cout<<" 2: DFS "<<endl;
        cout<<" 3: IDS "<<endl;
        cout<<" 4: Greedy "<<endl;
        cout<<" 5: UCS "<<endl;
        cout<<" 6: A* "<<endl;
        cout<<"-------------------------------------------------------"<<endl;
        cin>>algorithm;
    }while((algorithm<1)||(algorithm>6));
    
    cout<<"Please enter the starting city"<<endl;
    cin>>start_city;
    cout<<endl;
    cout<<"Please enter the ending city"<<endl;
    cin>>end_city;
    cout<<endl;
    
    if(start_city == end_city)
    {
        cout<<"You are already in your destination"<<endl;
        return 0;
    }
    
     cout<<"Please enter the file's name that contains the road names and their distances"<<endl;
     cin>>file_name_road;
    cout<<endl;
     cout<<"Please enter the file's name that contains the road names and their coordinates"<<endl;
     cin>>file_name_pos;
    file_name_temp = "temp.txt";
    

    
    /////////adds to the file the second direction eg: (A B 40) -> (B A 40)///////
    int position_1, position_2, cost;
    string line, city_1, city_2, cost_str;
    ofstream outfile("temp.txt");
    ifstream myfile(file_name_road);
    
    
    while (getline (myfile,line))
    {
        
        position_1 = line.find_first_of(" ");
        position_2 = line.find_last_of(" ");
        
        city_1 = line.substr(1,position_1 - 1); //city_1's name
        city_2 = line.substr(position_1 + 1, (position_2 - position_1) - 1);
        //cost
        cost_str = line.substr(position_2+1, line.length() - 1);
        cost = stoi(cost_str);
        //city_2's name
        outfile<<"("<<city_2<<" "<<city_1<<" "<<cost<<")"<<endl;
    }
    myfile.close();
    
    // open again and put in
    
    myfile.open(file_name_road);
    
    
    while (getline (myfile,line))
    {
        
        outfile<<line<<endl;
    }
    myfile.close();
    
    
    //////////////////////////////////////////////////////////
    
    //make start_city the root
    node *root = newNode(start_city, 0, "", NULL, 0, 0);
    
    
    //call fns
    if(algorithm == 1)
    {
        BFS(file_name_temp, root, end_city);
    }
    if(algorithm == 2)
    {
        //DFS(file_name_road, root, end_city);
        DFS(file_name_temp, root, end_city);
    }
    if(algorithm == 3)
    {
        cout<<"Please enter the limit"<<endl;
        cin>>limit;
        IDS(file_name_temp, root, end_city, limit);
    }
    if((algorithm == 4) || (algorithm == 5) || (algorithm == 6))
    {
        UCS_Greedy_AStar(file_name_temp, file_name_pos, root, end_city, algorithm);
    }
    return 0;
}
