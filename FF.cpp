#include<bits/stdc++.h>
using namespace std;

int first_fit(vector<int> order, int n, vector<vector<int>> Graph){
    vector<int> which_color(n+1, 0);
    int max_color=0;
    for(int adding: order){
        set<int> clours;

        //colour list
        for(int j : Graph[adding]){
            if(which_color[j] != 0){
                  clours.insert(which_color[j]);
            }
        }
        
        //color assignment
        for(int color=1;color<=n;color++){
            if(clours.count(color)==0){
                which_color[adding]=color;
                max_color=max(max_color,color);
                break;
            }
    
        }
   }


   return max_color;
}


int func(int n, vector<vector<int>> Graph){
    vector<int> v;
    for(int i=1;i<=n;i++){
        v.push_back(i);
    }   

    int worst = 0;
    do{
        int ans =  first_fit(v, n, Graph);
        worst = max(ans, worst);
    }while(next_permutation(v.begin(), v.end()));

    return worst;
}


int optimal(vector<vector<int>> Graph){
    
}

int main(){
    return 0;
}