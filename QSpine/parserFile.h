#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <qvector3d.h>
struct pozvonok
{
    std::string spine_name;
    QVector3D position;
    QVector3D rotation;
};

class parserFile
{
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;
    std::vector<std::vector<int>> index;
    std::vector<std::string> spine_name;
public:
    parserFile(std::string file_path) {
        std::ifstream file("SDL_VBL.txt"); // ������� ���� ��� ������
        if (file.is_open()) { // ���������, ��� ���� ������� ������
            std::string line;
            while (std::getline(file, line)) { // ������ ���� ���������
                size_t pos = line.find("index");
                if (pos != std::string::npos) {
                    line.erase(pos, std::string("index=[").length());
                    pos = line.find("]");
                    line.erase(pos, std::string("];").length());
                    std::istringstream ss(line);
                    std::vector<int> vec;
                    int num;
                    while (ss >> num) { // ������ ����� �� ������
                        vec.push_back(num);
                    }


                    for (int i = 0; i < vec.size(); i += 3) { // ���������� �� ������� �� ��� �����
                        std::vector<int> tmp;
                        for (int j = 0; j < 3; j++) {
                            tmp.push_back(vec[i + j]);
                        }
                        index.push_back(tmp);
                    }
                    continue;//��������� ������
                }
                pos = line.find("x");//����� ������� x
                if (pos != std::string::npos) {// ���� ������ 
                    line.erase(pos, std::string("x=[").length());//�������� x=[ �� ������
                    pos = line.find("]");
                    line.erase(pos, std::string("];").length());//�������� ] �� ������
                    std::istringstream iss(line); // �������� ������ ����� �� ������
                    double num;
                    while (iss >> num) { // ������ ����� �� ������
                        x.push_back(num); // ���������� ����� � ������
                    }

                    continue;//��������� ������
                }
                pos = line.find("y");
                if (pos != std::string::npos) {
                    line.erase(pos, std::string("y=[").length());
                    pos = line.find("]");
                    line.erase(pos, std::string("];").length());
                    std::istringstream iss(line); // �������� ������ ����� �� ������
                    double num;
                    while (iss >> num) { // ������ ����� �� ������
                        y.push_back(num); // ���������� ����� � ������
                    }
                    continue;//��������� ������
                }
                pos = line.find("z");
                if (pos != std::string::npos) {
                    line.erase(pos, std::string("z=[").length());
                    pos = line.find("]");
                    line.erase(pos, std::string("];").length());
                    std::istringstream iss(line); // �������� ������ ����� �� ������
                    double num;
                    while (iss >> num) { // ������ ����� �� ������
                        z.push_back(num); // ���������� ����� � ������
                    }
                    continue;//��������� ������
                }
                pos = line.find("spine_name");
                if (pos != std::string::npos) {
                    line.erase(pos, std::string("spine_name=[").length());
                    pos = line.find("]");
                    line.erase(pos, std::string("];").length());
                    std::istringstream iss(line);

                    std::string substring;
                    while (iss >> substring) {
                        spine_name.push_back(substring);
                    }
                    continue;//��������� ������
                }

            }
            file.close(); // ������� ����
        }
    }

    std::vector<pozvonok> get_spine() {
        std::vector<pozvonok> res;
        for (size_t i = 0; i < spine_name.size(); i++)
        {
            pozvonok pz;
            pz.spine_name = spine_name[i];
            int ind = index[i][1];
            if (ind > x.size()-1) {
                ind = x.size()-1;
            }
            pz.position = QVector3D(x[ind]*0.2f,y[ind] * 0.2f,z[ind] * 0.2f);
            pz.rotation = QVector3D(0,0,0);
            res.push_back(pz);
        }
        return res;
    }
    
};