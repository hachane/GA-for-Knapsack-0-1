#include <bits/stdc++.h>
using namespace std;
const int pop = 100; // số lượng cá thể
const int gen = 10; // số lượng thế hệ sẽ xét đến
const int N = 50; // số lượng món đồ nhiều nhất trong các testcase đang xét

vector<vector<bool>> chromosomes, offspring; // các vector lưu lại các NST của các cá thể thế hệ hiện tai; và thế hệ con
int n, capacity; // n là số lượng món đồ đang xét trong test case hiện tại
int items_volume[N], items_benefit[N], fitness[pop], total_fitness;
unordered_map<int, int> fitness_cnt;

// khởi tạo quần thể; mỗi quần thể có dạng 010..001
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

// lưu lại nhũng món đồ có "mật độ giá trị" cao nhất bằng 1 vecto gồm các pair có tên là midu.
// midu.first là mật độ giá trị của món đồ i = giá tiền / khối lượng.
// midu.second là chỉ số i (đánh dấu chỉ số) => sort theo midu.first thành 1 dãy giảm dần theo mật độ giá trị.
//lấy những món đồ có mật độ giá trị cao cho đến khi khối lượng vượt quá sức chứa
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
    vector<int> temp;
    int k = 0;
    int sum = 0;
    while(k < midu.size() && sum + items_volume[midu[k].second] <= capacity)
    {
        temp.push_back(midu[k].second);
        sum = sum + items_volume[midu[k].second];
        k++;
    }
    for(int i = 0; i < n; i++)
     chromosomes[chromosome_index][i] = 0;
    for(int i = 0; i < k; i++)
     chromosomes[chromosome_index][temp[i]] = 1;
}

/* tính toán độ fitness; trong bài toján đang xét thì fitness = tổng giá trị các món đồ; lưu những giá trị fitness vào 1 map để xét đến tần suất xuất hiện của những 
cá thể trùng độ fitness và tính tần suất xuất hiện của chúng. Nếu một cá thể mà có tổng khối lượng vượt quá sức chứa; thì ta sẽ chọn ngẫu nhiên 1 món đồ để bỏ lại 
trong phương án ấy. */
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
            {
                total_volume += items_volume[j];
                total_benefit += items_benefit[j];
            }
        }
        if (total_volume > capacity) {
            while (total_volume > capacity) 
            {
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

// Sau khi biết được tần suất xuất hiện của các fitness có thể thu được; thì ta xem tần suất đấy xuất hiện bao nhiêu phần trăm trong tổng số tất cả các cá thể. 
// Nếu quá 90% dân số có cùng độ fitness thì ta dừng thuật toán.
int calc_percentage()
{
    int max_cnt = 0;
    for(auto x: fitness_cnt)
     max_cnt = max(max_cnt, x.second);
    return max_cnt * 100 / pop;
}

// tìm cá thể có độ fitness cao nhất; cao thứ 2
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

//elitism là lưu lại 2 cá thể tốt nhất từ quần thể cũ sang quần thể mới
void elitism()
{
    int fittest_index = get_fittest();
    offspring.push_back(chromosomes[fittest_index]);
    int second_fitness_index = get_second_fitness(fittest_index);
    offspring.push_back(chromosomes[second_fitness_index]);
}

// toán tử chọn lọc: sử dụng bánh xe roulette; random đến 1 giá trị r nhỏ hơn total_fitness rồi tiến hành cộng các fitness đến khi nào > r thì dừng
// cá thể nào có độ fitness càng lớn càng dễ được chọn!! (do vùng fitness "rộng" hơn)
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

// đột biến: 2 cách đột biến; đột biến điểm và đột biến đảo đoạn
void mutation(vector<bool> &chromosome)
{
    for(int i = 0; i < n; i++)
    {
        int r = rand() % 1000;
        if(r < 1000)
         chromosome[i] != chromosome[i];
    }
    /*int r = rand() % 1000;
    if(r < 1000)
    {
        int r1 = rand() % (n - 1);
        int r2 = rand() % (n - 1);
        for (int k = 0; k < (max(r1, r2) - min(r1, r2))/2; k++)
        {
            int temp = chromosome[min(r1, r2) + k];
            chromosome[min(r1,r2) + k] = chromosome[max(r1,r2) - k];
            chromosome[max(r1,r2) - k] = temp;
        }
     }*/
}

// toán tử lai ghép: chọn ngẫu nhiên 1 số r < n (n giờ đây còn là độ dài DNA) là điểm bắt đầu trao đổi chéo
void crossover(int chromosome_index1, int chromosome_index2)
{
    vector<bool> chromosome1 = chromosomes[chromosome_index1];
    vector<bool> chromosome2 = chromosomes[chromosome_index2];
    int r = rand() % n;
    for (int i = 0; i < r; i++)
     swap(chromosome1[i], chromosome2[i]);
    mutation(chromosome1);
    mutation(chromosome2);
    /*int r1 = rand() % 1000;
    if(r1 < 1000)
    {
        greedy(chromosome_index1);
        greedy(chromosome_index2);
    }*/
    offspring.push_back(chromosome1);
    offspring.push_back(chromosome2);
}

// tiến hành sinh sản => 2 cách chọn bố mẹ: luôn chọn 2 cá thể tốt nhất hoặc chọn ngẫu nhiên
void reproduce()
{
    elitism();
    while (offspring.size() < pop)
    {
        int chromosome_index1 = roulette_wheel_selection() /* fittest_index() */;
        int chromosome_index2 = roulette_wheel_selection() /* get_second_fitness(fittest_index()) */;
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
    for (int c = 1; c <= t; c++)
    {
        offspring.clear();
        cin >> n >> capacity;
        for (int i = 0; i < n; i++)
         cin >> items_volume[i] >> items_benefit[i];
        generate_population();
        int percentage = 0, generation_number = 0;
        while (generation_number < gen || percentage < 90)
        {
            if (generation_number)
            {
                chromosomes = offspring;
                offspring.clear();
            }
            calc_fitness();
            percentage = calc_percentage();

            generation_number++;
            if (generation_number < gen || percentage < 90)
                reproduce();
        }
        int fittest_index = get_fittest();
        int res = fitness[fittest_index];
        cout << "Case " << c << ": " << res << " ";
        cout << endl;
    }
    return 0;
}
