#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QSpinBox>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QApplication>
#include <QDateTime>
#include <QTableWidget>
#include <QString>
#include <QTextEdit>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <queue>
#include <unistd.h>
#include <algorithm>
#include <vector>
#include <set>

using namespace std;

static MainWindow * mw;

int elem_n = 0, ne = 0;

set<pair<int, string>> dnf;
vector<vector<string>> perm(6);
vector<vector<int>> used(65, vector<int>(65, 0));
vector<vector<string>> table(65, vector<string>(65));

QString bin_num = "";
QTableWidget *main_table;
QTextEdit *num_change;
QGridLayout *main_window;

void gen_perm() {
    set<pair<int, string>> st;
    for (char c1 = '`'; c1 <= 'f'; ++c1) {
        for (char c2 = '`'; c2 <= 'f'; ++c2) {
            for (char c3 = '`'; c3 <= 'f'; ++c3) {
                for (char c4 = '`'; c4 <= 'f'; ++c4) {
                    for (char c5 = '`'; c5 <= 'f'; ++c5) {
                        for (char c6 = '`'; c6 <= 'f'; ++c6) {
                            string s = "";
                            if (c1 != '`') { s += c1; }
                            if (c2 != '`' && c2 != c1) { s += c2; }
                            if (c3 != '`' && c3 != c2 && c3 != c1) { s += c3; }
                            if (c4 != '`' && c4 != c1 && c2 != c4 && c4 != c3) { s += c4; }
                            if (c5 != '`' && c5 != c1 && c5 != c4 && c5 != c3 && c5 != c2) { s += c5; }
                            if (c6 != '`' && c6 != c5 && c6 != c4 && c6 != c3 && c6 != c2 && c6 != c1) { s += c6; }
                            sort(s.begin(), s.end());

                            st.insert({s.size(), s});
                        }
                    }
                }
            }
        }
    }
    for (auto i : st) {
        if (i.second.find('f') != string::npos) {
            perm[5].push_back(i.second);
        } else if (i.second.find('e') != string::npos) {
            perm[4].push_back(i.second);
            perm[5].push_back(i.second);
        } else if (i.second.find('d') != string::npos) {
            perm[3].push_back(i.second);
            perm[4].push_back(i.second);
            perm[5].push_back(i.second);
        } else if (i.second.find('c') != string::npos) {
            perm[2].push_back(i.second);
            perm[3].push_back(i.second);
            perm[4].push_back(i.second);
            perm[5].push_back(i.second);
        } else if (i.second.find('b') != string::npos) {
            perm[1].push_back(i.second);
            perm[2].push_back(i.second);
            perm[3].push_back(i.second);
            perm[4].push_back(i.second);
            perm[5].push_back(i.second);
        } else {
            perm[0].push_back(i.second);
            perm[1].push_back(i.second);
            perm[2].push_back(i.second);
            perm[3].push_back(i.second);
            perm[4].push_back(i.second);
            perm[5].push_back(i.second);
        }
    }
}

void render_table(int num_of_elem) {
    ne = num_of_elem;
    int n = (1 << num_of_elem);
    elem_n = n;
    main_table->setRowCount(n + 1);
    main_table->setColumnCount(n);

    for (int i = 0; i < n; ++i) {
        QString s = QString::fromStdString(perm[num_of_elem - 1][i]);
        QTableWidgetItem *newItem = new QTableWidgetItem();
        newItem->setText(s);
        main_table->setItem(0, i, newItem);
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            string s = "";
            for (char k : perm[num_of_elem - 1][j]) {
                for (int c = 0; c <= 5; ++c) {
                    if (k == c + 'a') {
                        s += (i >> (num_of_elem - c - 1) & 1) + '0';
                    }
                }
            }
            table[i][j] = s;

            QTableWidgetItem *newItem = new QTableWidgetItem();
            newItem->setText(QString::fromStdString(s));
            main_table->setItem(i + 1, j, newItem);
        }
    }
}

void find_dnf() {
    set<string> used;
    set<pair<int, int>> v;
    map<string, vector<pair<int, int>>> m;
    vector<string> bin;
    for (int i = 0; i < elem_n; ++i) {
        for (int j = 0; j < elem_n; ++j) {
            string s = perm[ne - 1][j] + table[i][j];
            if (table[i][j] != "#" && used.find(s) == used.end()) {
                bin.push_back(s);
                for (int k = 0; k < elem_n; ++k) {
                    if (table[i][j] == table[k][j]) {
                        v.insert(make_pair(k, j));
                        m[s].emplace_back(k, j);
                    }
                }
                used.insert(perm[ne - 1][j] + table[i][j]);
            }
        }
    }

    int n = bin.size();
    for (int mask = 0; mask < (1 << n); ++mask) {
        vector<string> tmp;
        for (int bit = 0; bit < n; ++bit) {
            if (mask >> bit & 1) {
                tmp.push_back(bin[bit]);
            }
        }

        set<int> st;
        vector<string> tmp2;
        for (auto i : tmp) {
            for (auto j : m[i]) {
                st.insert(j.first);
            }
            tmp2.push_back(i);
        }

        if (st.size() == count(bin_num.begin(), bin_num.end(), '1')) {
            string ans = "";
            for (auto i : tmp2) {
                int l = i.length();
                for (int j = 0; j < l / 2; ++j) {
                    if (i[j + l / 2] == '0') {
                        ans += "!";
                        ans += i[j];
                    } else {
                        ans += i[j];
                    }
                }
                ans += " ⋁ ";
            }
            dnf.insert({ans.length(), ans});
        }
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    mw = this;

    main_window = new QGridLayout;

    main_table = new QTableWidget;
    // header; num entering, labels and button
    QLabel *num_input = new QLabel("Number:");
    num_input->setStyleSheet("font-weight:bold;font-size:20px;");
    main_window->addWidget(num_input, 0, 0);

    num_change = new QTextEdit;
    num_change->setFixedSize(200, 25);
    main_window->addWidget(num_change, 0, 1);

    QLabel *num_elm = new QLabel("Variables:");
    num_elm->setStyleSheet("font-weight:bold;font-size:20px;");
    main_window->addWidget(num_elm, 0, 2);

    QSpinBox *num_elm_change = new QSpinBox;
    main_window->addWidget(num_elm_change, 0, 3);
    num_elm_change->setMaximum(6);

    QPushButton *button = new QPushButton("&Go for DNF!");
    main_window->addWidget(button, 0, 4);

    //
    gen_perm();

 /*   QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(
           (6+1)*(2+1)));
    main_table->setItem(6, 2, newItem);

*/

    main_window->addWidget(main_table, 1, 0, 30, 10);
    ui->verticalLayout->addLayout(main_window);

    // connect

  //  connect(num_change, QOverload<int>::of(&QSpinBox::valueChanged), );
    connect(num_elm_change, QOverload<int>::of(&QSpinBox::valueChanged), render_table);
    QObject::connect(num_change, SIGNAL(textChanged()), this, SLOT(render_bin()));
    QObject::connect(button, SIGNAL(pressed()), this, SLOT(clickedSlot()));
   // connect(num_change, &QTextEdit::textChanged, render_bin);
}

void MainWindow::clickedSlot() {
    dnf.clear();
    render_table(ne);
    if (!bin_num.size()) {
        QLabel *inv = new QLabel("invalid input");
        main_window->addWidget(inv, 100, 0);
    } else {
        for (int i = 0; i < elem_n; ++i) {
            if (bin_num[i] == '0') {
                for (int j = 1; j < elem_n; ++j) {
                    for (int k = 0; k < elem_n; ++k) {
                        if (k != i && table[k][j] == table[i][j]) {
                            //QTableWidgetItem *newItem = new QTableWidgetItem();
                            //newItem->setText(QString::fromStdString("#"));
                            //main_table->setItem(k + 1, j, newItem);
                            main_table->item(k + 1, j)->setBackgroundColor(Qt::red);
                            table[k][j] = "#";
                        }
                    }

                    /*QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setText(QString::fromStdString("#"));
                    main_table->setItem(i + 1, j, newItem);*/
                    main_table->item(i + 1, j)->setBackgroundColor(Qt::blue);
                    table[i][j] = "#";
                }
            }
        }

        find_dnf();

        set<string> final_res;
        int f = 0, prev = (*dnf.begin()).first;
        for (auto i : dnf) {
            if (i.first > prev) { break; }
            final_res.insert(i.second.substr(0, i.second.size() - 4));
        }

        for (auto i : final_res) {
            QLabel *ans = new QLabel(QString::fromStdString(i));
            main_window->addWidget(ans, 100 + f, 0, 10, 10);
            ++f;
        }

    }
}


void MainWindow::render_bin() {
    QString s = num_change->toPlainText();
    if (s.size() <= elem_n) {
        string num = string(elem_n - s.size(), '0');
        QString cur = QString::fromStdString(num) + s;
        bin_num = cur;
        QTableWidgetItem *newItem = new QTableWidgetItem();
        newItem->setText(cur);

        for (int i = 0; i < elem_n; ++i) {
            QTableWidgetItem *newItem = new QTableWidgetItem();
            newItem->setText(QString(cur[i]));
            main_table->setItem(i + 1, 0, newItem);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
