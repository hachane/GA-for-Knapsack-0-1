#include <bits/stdc++.h>
using namespace std;
const int pop = 100; // so luong ca the
const int gen = 10; // so luong the he dang xet
const int N = 50; // so luong mon do nhieu nhat trong cac testcase dang xet

vector<vector<bool>> chromosomes, offspring; // cac vector luu lai NST; ca the con
int n, capacity, items_volume[N], items_benefit[N], fitness[pop], total_fitness;
unordered_map<int, int> fitness_cnt;

//khoi tao quan the bang nhung day nhi phan 01001...01000
void generate_population()
{
    chromosomes.resize(pop);
    for(int i = 0; i < pop; i++)
    {
        chromosomes[i].resize(n);
        for (int j = 0; j < n; j++)
        {
            int r = rand() % 10;
            if(r < 5)
             chromosomes[i][j] = 1;
        }
    }
}

void greedy(int chromosome_index)
{
    vector<pair<double, int>> midu;
    for(int i = 0; i < n; i++)
    {
        if(chromosomes[chromosome_index][i] == 1)
         midu.push_back(make_pair((items_benefit[i] * 1.0) / items_volume[i], i));
        else
         midu.push_back(make_pair(0, i));
    }
    sort(midu.rbegin(), midu.rend());
    vector<bool> temp;
    int track = 0;
    int k = 0;
    int sum = 0;
    while(sum <= capacity)
    {
        sum += items_volume[midu[k].second];
        temp.push_back(midu[k].second);
        k++;
        track++;
    }
    for(int i = 0; i < n; i++)
     chromosomes[chromosome_index][i] = 0;
    for(int i = 0; i < track; i++)
     chromosomes[chromosome_index][temp[i]] = 1;
}
void calc_fitness()
{
    total_fitness = 0;
    fitness_cnt.clear();
    for(int i = 0; i < pop; i++)
    {
        int total_benefit = 0, total_volume = 0;
        for(int j = 0; j < n; j++)
        {
            if(chromosomes[i][j])
             {total_volume += items_volume[j];
             total_benefit += items_benefit[j];}
            }

        if (total_volume > capacity) {
            while (total_volume > capacity) {
                int r = rand() % n;
                while (chromosomes[i][r] == 0)
                    r = rand() % n;
                chromosomes[i][r] = 0;
                total_volume -= items_volume[r];
                total_benefit -= items_benefit[r];
            }
        }

        fitness[i] = total_benefit;
        fitness_cnt[fitness[i]]++;
        total_fitness += fitness[i];
    }
}
int calc_percentage()
{
    int max_cnt = 0;
    for(auto x: fitness_cnt)
     max_cnt = max(max_cnt, x.second);
    return max_cnt * 100 / pop;// tim phan tu chiem fitness nhieu nhat
}
int get_fittest()
{
    int max_fitness = INT_MIN, index = -1;
    for(int i = 0; i < pop; i++)
    {
        if(max_fitness < fitness[i])
        {
            max_fitness = fitness[i];
            index = i;
        }
    }
    return index;
}
int get_second_fitness(int fittest_index)
{
    int max_second_fitness = INT_MIN, index = -1;
    for(int i = 0; i < pop; i++)
    {
        if(i != fittest_index && max_second_fitness <fitness[i])
        {
            max_second_fitness = fitness[i];
            index = i;
        }
    }
    return index;
}
void elitism()
{
    int fittest_index = get_fittest();
    offspring.push_back(chromosomes[fittest_index]);
    int second_fitness_index = get_second_fitness(fittest_index);
    offspring.push_back(chromosomes[second_fitness_index]);
}
int roulette_wheel_selection()
{
    int r = rand() % total_fitness, sum = 0;
    for(int i = 0; i < pop; i++)
    {
        sum += fitness[i];
        if(sum > r)
         return i;
    }
    return -1;
}
void mutation(vector<bool> &chromosome)
{
    for(int i = 0; i < n; i++)
    {
        int r = rand() % 1000;
        if(r < 1000)
         chromosome[i] != chromosome[i];
    }
}
void crossover(int chromosome_index1, int chromosome_index2)
{
    vector<bool> chromosome1 = chromosomes[chromosome_index1];
    vector<bool> chromosome2 = chromosomes[chromosome_index2];
    int r = rand() % n;
    for (int i = 0; i < r; i++)
     swap(chromosome1[i], chromosome2[i]);
    greedy(chromosome_index1);
    greedy(chromosome_index2);
    mutation(chromosome1);
    mutation(chromosome2);
    offspring.push_back(chromosome1);
    offspring.push_back(chromosome2);
}
void reproduce()
{
    //Elitism - two of the fittest chromosomes are copied without changes to a new population
    elitism();
    while (offspring.size() < pop)
    {
        int chromosome_index1 = roulette_wheel_selection();
        int chromosome_index2 = roulette_wheel_selection();
        crossover(chromosome_index1, chromosome_index2);
    }
}
int main()
{
    freopen("input.txt", "rt", stdin);
    freopen("output.txt", "wt", stdout);
    srand(time(NULL));
    int t;
    cin >> t;
    for (int c = 0; c < t; c++)
    {
        offspring.clear();
        cin >> n >> capacity;
        for (int i = 0; i < n; i++)
         cin >> items_volume[i] >> items_benefit[i];
        /*vector<bool> best;
        for(int i = 0; i < n; i++)
        {
         bool element; cin >> element;
         best.push_back(element);
        }*/
        generate_population();
        int percentage = 0, generation_number = 0;
        while (percentage < 90 || generation_number < gen)
        {
            if (generation_number)
            {
                chromosomes = offspring;
                offspring.clear();
            }
            calc_fitness();
            percentage = calc_percentage();
            generation_number++;
            // think if the percentage will be lost !!
            if (percentage < 90 || generation_number < gen)
                reproduce();
        }
        int fittest_index = get_fittest();
        int res = fitness[fittest_index];
        cout << "Case: " << c << ' ' << res << " ";
        cout << endl;
        int diff = 0;
        for(int i = 0; i < n; i++)
        {
         /*if(best[i] != chromosomes[fittest_index][i]) diff++;*/
         cout << chromosomes[fittest_index][i] << " ";
        }
        cout << endl;
        /*cout << endl << (diff * 100.0) / n << endl;*/
    }
    return 0;
}
