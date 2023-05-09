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
        std::ifstream file(file_path); // открыть файл для чтения
        if (file.is_open()) { // проверить, что файл успешно открыт
            std::string line;
            while (std::getline(file, line)) { // читать файл построчно
                size_t pos = line.find("index");
                if (pos != std::string::npos) {
                    line.erase(pos, std::string("index=[").length());
                    pos = line.find("]");
                    line.erase(pos, std::string("];").length());
                    std::istringstream ss(line);
                    std::vector<int> vec;
                    int num;
                    while (ss >> num) { // Чтение чисел из строки
                        vec.push_back(num);
                    }


                    for (int i = 0; i < vec.size(); i += 3) { // Разделение на векторы по три числа
                        std::vector<int> tmp;
                        for (int j = 0; j < 3; j++) {
                            tmp.push_back(vec[i + j]);
                        }
                        index.push_back(tmp);
                    }
                    continue;//следующая строка
                }
                pos = line.find("x");//поиск символа x
                if (pos != std::string::npos) {// если найден 
                    line.erase(pos, std::string("x=[").length());//удаление x=[ из строки
                    pos = line.find("]");
                    line.erase(pos, std::string("];").length());//удаление ] из строки
                    std::istringstream iss(line); // Создание потока ввода из строки
                    double num;
                    while (iss >> num) { // Чтение чисел из потока
                        x.push_back(num); // Добавление числа в вектор
                    }

                    continue;//следующая строка
                }
                pos = line.find("y");
                if (pos != std::string::npos) {
                    line.erase(pos, std::string("y=[").length());
                    pos = line.find("]");
                    line.erase(pos, std::string("];").length());
                    std::istringstream iss(line); // Создание потока ввода из строки
                    double num;
                    while (iss >> num) { // Чтение чисел из потока
                        y.push_back(num); // Добавление числа в вектор
                    }
                    continue;//следующая строка
                }
                pos = line.find("z");
                if (pos != std::string::npos) {
                    line.erase(pos, std::string("z=[").length());
                    pos = line.find("]");
                    line.erase(pos, std::string("];").length());
                    std::istringstream iss(line); // Создание потока ввода из строки
                    double num;
                    while (iss >> num) { // Чтение чисел из потока
                        z.push_back(num); // Добавление числа в вектор
                    }
                    continue;//следующая строка
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
                    continue;//следующая строка
                }

            }
            file.close(); // закрыть файл
        }
       // std::reverse(spine_name.begin(), spine_name.end());
 
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
            pz.position = QVector3D(x[ind]*0.17f,y[ind] * 0.17f,z[ind] * 0.17f);
            
            int indP1 = index[i][0];
            int indP2 = index[i][2];
            QVector3D verticalVector(0.0, 1.0, 0.0); // Вертикальный вектор
            QVector3D pointA(x[indP1], y[indP1], z[indP1]); // Первая точка
            QVector3D pointB(x[indP2], y[indP2], z[indP2]); // Вторая точка

            QVector3D direction = (pointB - pointA).normalized(); // Направление между точками
            QQuaternion rotation = QQuaternion::rotationTo(verticalVector, direction); // Получение кватерниона поворота

            pz.rotation = rotation.toEulerAngles();

            pz.rotation = QVector3D(pz.rotation.x(), pz.rotation.y(), pz.rotation.z());
            
            res.push_back(pz);
        }

        float x = 0, y = 0, z = 0;
        for (size_t i = 0; i < res.size(); i++)
        {
            x += res[i].position.x();
            y += res[i].position.y();
            z += res[i].position.z();
        }
        x /= res.size();
        y /= res.size();
        z /= res.size();
        for (size_t i = 0; i < res.size(); i++)
        {
            res[i].position.setX(res[i].position.x() - x);
            res[i].position.setY(res[i].position.y() - y);
            res[i].position.setZ(res[i].position.z() - z);
        }



        return res;
    }
    
};