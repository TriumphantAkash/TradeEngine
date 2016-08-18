#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

using namespace std;
class order_entry{
    public:
        string order;   //BUY/SELL
        string order_type; //IOC/GFD
        int price;
        int quantity;
        int rank;   //used during trading, BUY/SELL orders are traded in the order of their rank, if order is modified, its rank gets worse
};

vector<string> split(string str, char delimiter){
    vector<string> internal;
    stringstream ss(str);
    string tok;
    while(getline(ss, tok, delimiter)){
        internal.push_back(tok);
    }
    return internal;
}
/*
void add_record(vector<string> v, unordered_map<string, order_entry> orders_passed){
    if(v[1].compare == "GFD"){
        //insert it in orders map
        order_entry oe;
        oe.order = v[0];
        oe.order_type = v[1];
        oe.price = atoi(v[2].c_str());
        oe.quantity = atoi(v[3].c_str());
        oe.rank = buy_rank;
        orders_passed[v[4]] = oe;

       // current_trade_flag = "BUY";
        buy_rank++;
    }
    else{
        //ignore this order
    }
}
*/

int main() {
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    unordered_map<string, order_entry> orders;
    string current_trade_flag = "xyz";
    string line;
    int buy_rank = 0;
    int sell_rank = 0;
    //cout<<"WELCOME"<<endl;
    while(getline(cin, line)){
        if(line.empty()){
            break;
        }
        if(line.compare("PRINT") == 0){
            //store buy and sell orders separately in separate multimaps with price as key so as to print them in decreasing order of prices
            map<int, int> buy_multimap;
            map<int, int> sell_multimap;
            for(pair<string, order_entry> element_pair: orders){
                if(element_pair.second.order.compare("BUY") == 0){
                    //store it in buy MAP
                    int price = element_pair.second.price;
                    int quantity = element_pair.second.quantity;
                    if(buy_multimap.find(price) == buy_multimap.end()){ //this element does not exist already so insert it
                        buy_multimap.insert(pair<int,int>(price, quantity));
                    }else{
                        //update the previous value for the same key with cumulative quantity
                        buy_multimap[price] = buy_multimap[price]+quantity;
                    }

                }else if(element_pair.second.order.compare("SELL") == 0){
                    //store it in sell map
                    int price = element_pair.second.price;
                    int quantity = element_pair.second.quantity;
                    if(sell_multimap.find(price) == sell_multimap.end()){ //this element does not exist already so insert it
                        sell_multimap.insert(pair<int,int>(price, quantity));
                    }else{
                        //update the previous value for the same key with cumulative quantity
                        sell_multimap[price] = sell_multimap[price]+quantity;
                    }

                }
            }
            //now print buy and sell in the desired order
            cout<<"SELL:"<<endl;
            map<int, int>::reverse_iterator itr1 = sell_multimap.rbegin();
            while(itr1 != sell_multimap.rend()){
                cout<<itr1->first<<" "<<itr1->second<<endl;
                ++itr1;
            }

            cout<<"BUY:"<<endl;
            itr1 = buy_multimap.rbegin();
            while(itr1 != buy_multimap.rend()){
                cout<<itr1->first<<" "<<itr1->second<<endl;
                ++itr1;
            }
        }else{
            vector<string> sep = split(line, ' ');
            if(sep[0].compare("CANCEL") == 0){
                //REMOVE THIS ORDER FROM orders
                orders.erase(sep[1]);   //second column is order id in case of cancel
            }else if(sep[0].compare("MODIFY") == 0){
                //modify this element in the map
                unordered_map<string, order_entry>::iterator i = orders.find(sep[1]);
                if(i != orders.end()){ //element is present in the map
                    order_entry oe = i->second;
                    if(sep[2].compare("BUY") == 0){
                        oe.rank = buy_rank; //later we gonna use this rank to decide trade priority
                        oe.order = "BUY";
                    }else if(sep[2].compare("SELL") == 0){
                        oe.rank = sell_rank;
                        oe.order = "SELL";
                    }else{
                        //incorrect input
                    }
                    oe.price = atoi(sep[3].c_str());
                    oe.quantity = atoi(sep[4].c_str());
                    //insert oe back to the orders map
                    //orders[sep[1]] = oe;
                    orders[sep[1]] = oe;
                }else{
                    //element is not present in map
                }
            }else if((sep[0].compare("BUY") == 0) || (sep[0].compare("SELL") == 0)){
                int buy_count = 0;
                int sell_count = 0;
                for(pair<string, order_entry> element_pair: orders){
                    if(element_pair.second.order.compare("BUY") == 0){
                        //count # of buy orders
                        buy_count++;
                    }else if(element_pair.second.order.compare("SELL") == 0){
                       //count # of sell orders
                        sell_count++;
                    }else{
                        //not possible
                    }
                }

                if(sep[0].compare("BUY") == 0){

                    int buy_quantity = atoi(sep[3].c_str());
                    int buy_price = atoi(sep[2].c_str());
                    string buy_order_id = sep[4];

                    if(sell_count > 0){
                        map<int, string> rank_based_orders;

                        for(pair<string, order_entry> entry: orders){   //add orders to map so as to sort them based on rank and later use this map to do trading
                             if(entry.second.order.compare("SELL") == 0){   //cuz we need to do trading only on SELL, because current input is BUY
                                rank_based_orders.insert(pair<int,string>(entry.second.rank, entry.first));
                             }
                        }

                        //take out each order one by one and process it
                        map<int, string>::iterator rank_map_iterator = rank_based_orders.begin();
                        while(rank_map_iterator != rank_based_orders.end()){
                            unordered_map<string, order_entry>::iterator orders_iterator = orders.find(rank_map_iterator->second);
                            order_entry s_order_entry = orders_iterator->second;
                            string s_order_id = orders_iterator->first;
                            if(buy_price >= s_order_entry.price){   //main condition for trading
                                //print trade info
                                if(buy_quantity > 0){   //still some quantity left
                                    if(buy_quantity < s_order_entry.quantity){
                                        cout<<"TRADE "<<s_order_id<<" "<<s_order_entry.price<<" "<<buy_quantity<<" "<<buy_order_id<<" "<<buy_price<<" "<<buy_quantity<<endl;
                                        //update that partially traded sell entry in orders map
                                        s_order_entry.quantity = s_order_entry.quantity-buy_quantity;
                                        orders[s_order_id] = s_order_entry;
                                        buy_quantity = 0;
                                        break;  //don't need to iterate anymore as all quantity is consumed now
                                    }else if(buy_quantity > s_order_entry.quantity){
                                        //1) print trade info
                                        cout<<"TRADE "<<s_order_id<<" "<<s_order_entry.price<<" "<<s_order_entry.quantity<<" "<<buy_order_id<<" "<<buy_price<<" "<<s_order_entry.quantity<<endl;
                                        //2) remove sell entry from the hashMap
                                        orders.erase(s_order_id);
                                        //3) update buy quantity
                                        buy_quantity = buy_quantity - s_order_entry.quantity;
                                    }else{  //equal
                                        //1) print trade info
                                        cout<<"TRADE "<<s_order_id<<" "<<s_order_entry.price<<" "<<s_order_entry.quantity<<" "<<buy_order_id<<" "<<buy_price<<" "<<s_order_entry.quantity<<endl;
                                        //2) remove sell entry from the hashMap
                                        orders.erase(s_order_id);
                                        buy_quantity = 0;
                                    }
                                }else{
                                    //all quantity consumed
                                    break;
                                }
                            }else{
                                //this sell order can't be traded right now, just leave it
                            }
                            rank_map_iterator++;
                        }
                        if ((buy_quantity > 0) && (sep[1].compare("GFD") == 0)){   //still some quantity left, store it if it is GFD type else chuck it
                            order_entry new_buy_entry;
                            new_buy_entry.order = "BUY";
                            new_buy_entry.order_type = "GFD";
                            new_buy_entry.price = buy_price;
                            new_buy_entry.quantity = buy_quantity;
                            new_buy_entry.rank = buy_rank;

                            orders.insert(pair<string, order_entry>(buy_order_id, new_buy_entry));
                            buy_rank++;
                        }


                    }else{  //no sell entries ==> just insert this buy entry in the map

                        if(sep[1].compare("GFD") == 0){
                            order_entry new_buy_entry;
                            new_buy_entry.order = "BUY";
                            new_buy_entry.order_type = "GFD";
                            new_buy_entry.price = buy_price;
                            new_buy_entry.quantity = buy_quantity;
                            new_buy_entry.rank = buy_rank;

                            orders.insert(pair<string, order_entry>(buy_order_id, new_buy_entry));
                            buy_rank++;
                        }
                    }
                }else{  //SELL

                    int sell_quantity = atoi(sep[3].c_str());
                    int sell_price = atoi(sep[2].c_str());
                    string sell_order_id = sep[4];

                    if(buy_count > 0){
                        map<int, string> rank_based_orders;

                        for(pair<string, order_entry> entry: orders){   //add orders to map so as to sort them based on rank and later use this map to do trading
                             if(entry.second.order.compare("BUY") == 0){   //cuz we need to do trading only on SELL, because current input is BUY
                                rank_based_orders.insert(pair<int,string>(entry.second.rank, entry.first));
                             }
                        }

                        //take out each order one by one and process it
                        map<int, string>::iterator rank_map_iterator = rank_based_orders.begin();
                        while(rank_map_iterator != rank_based_orders.end()){
                            unordered_map<string, order_entry>::iterator orders_iterator = orders.find(rank_map_iterator->second);
                            order_entry b_order_entry = orders_iterator->second;
                            string b_order_id = orders_iterator->first;
                            if(sell_price <= b_order_entry.price){   //main condition for trading
                                //print trade info
                                if(sell_quantity > 0){   //still some quantity left
                                    if(sell_quantity < b_order_entry.quantity){
                                        cout<<"TRADE "<<b_order_id<<" "<<b_order_entry.price<<" "<<sell_quantity<<" "<<sell_order_id<<" "<<sell_price<<" "<<sell_quantity<<endl;
                                        //update that partially traded sell entry in orders map
                                        b_order_entry.quantity = b_order_entry.quantity-sell_quantity;
                                        orders[b_order_id] = b_order_entry;
                                        sell_quantity = 0;
                                        break;  //don't need to iterate anymore as all quantity is consumed now
                                    }else if(sell_quantity >b_order_entry.quantity){
                                        //1) print trade info
                                        cout<<"TRADE "<<b_order_id<<" "<<b_order_entry.price<<" "<<b_order_entry.quantity<<" "<<sell_order_id<<" "<<sell_price<<" "<<b_order_entry.quantity<<endl;
                                        //2) remove sell entry from the hashMap
                                        orders.erase(b_order_id);
                                        //3) update buy quantity
                                        sell_quantity = sell_quantity - b_order_entry.quantity;
                                    }else{  //equal
                                        //1) print trade info
                                        cout<<"TRADE "<<b_order_id<<" "<<b_order_entry.price<<" "<<b_order_entry.quantity<<" "<<sell_order_id<<" "<<sell_price<<" "<<b_order_entry.quantity<<endl;
                                        //2) remove sell entry from the hashMap
                                        orders.erase(b_order_id);
                                        sell_quantity = 0;
                                    }
                                }else{
                                    //all quantity consumed
                                    break;
                                }
                            }else{
                                //this sell order can't be traded right now, just leave it
                            }
                            rank_map_iterator++;
                        }
                        if ((sell_quantity > 0) && (sep[1].compare("GFD") == 0)){   //still some quantity left, store it if it is GFD type else chuck it
                            order_entry new_sell_entry;
                            new_sell_entry.order = "SELL";
                            new_sell_entry.order_type = "GFD";
                            new_sell_entry.price = sell_price;
                            new_sell_entry.quantity = sell_quantity;
                            new_sell_entry.rank = sell_rank;

                            orders.insert(pair<string, order_entry>(sell_order_id, new_sell_entry));
                            sell_rank++;
                        }


                    }else{  //no sell entries ==> just insert this buy entry in the map
                        if(sep[1].compare("GFD") == 0){
                            order_entry new_sell_entry;
                            new_sell_entry.order = "SELL";
                            new_sell_entry.order_type = "GFD";
                            new_sell_entry.price = sell_price;
                            new_sell_entry.quantity = sell_quantity;
                            new_sell_entry.rank = sell_rank;

                            orders.insert(pair<string, order_entry>(sell_order_id, new_sell_entry));
                            sell_rank++;
                        }
                    }
                }
            }else{
                //incorrect input
            }
         }//not print
        }
    return 0;
}
