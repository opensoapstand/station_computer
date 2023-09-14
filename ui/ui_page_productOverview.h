/********************************************************************************
** Form generated from reading UI file 'page_productOverview.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_PRODUCTOVERVIEW_H
#define UI_PAGE_PRODUCTOVERVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_page_product_overview
{
public:
    QPushButton *pushButton_previous_page;
    QPushButton *pushButton_continue;
    QPushButton *pushButton_to_help;
    QLabel *label_invoice_price;
    QLabel *label_invoice_price_total;
    QLabel *label_selected_volume;
    QLabel *label_selected_price;
    QLineEdit *lineEdit_promo_code;
    QPushButton *pushButton_promo_input;
    QWidget *promoKeyboard;
    QPushButton *num1;
    QPushButton *num2;
    QPushButton *num3;
    QPushButton *num4;
    QPushButton *num5;
    QPushButton *num6;
    QPushButton *num7;
    QPushButton *num8;
    QPushButton *num9;
    QPushButton *num0;
    QPushButton *q;
    QPushButton *w;
    QPushButton *e;
    QPushButton *r;
    QPushButton *t;
    QPushButton *y;
    QPushButton *u;
    QPushButton *i;
    QPushButton *o;
    QPushButton *p;
    QPushButton *a;
    QPushButton *s;
    QPushButton *d;
    QPushButton *g;
    QPushButton *f;
    QPushButton *h;
    QPushButton *j;
    QPushButton *k;
    QPushButton *l;
    QPushButton *z;
    QPushButton *x;
    QPushButton *c;
    QPushButton *v;
    QPushButton *b;
    QPushButton *n;
    QPushButton *m;
    QPushButton *backspace;
    QPushButton *done;
    QLabel *label_keyboard_background;
    QLabel *label_invoice_name;
    QLabel *label_invoice_discount_amount;
    QLabel *label_invoice_discount_name;
    QLabel *label_product_photo;
    QLabel *label_discount_tag;
    QPushButton *pushButton_select_product_page;
    QLabel *label_invoice_box;
    QLabel *label_gif;
    QLabel *label_total;
    QLabel *label_help;
    QFrame *line_invoice;
    QButtonGroup *buttonGroup;

    void setupUi(QWidget *page_product_overview)
    {
        if (page_product_overview->objectName().isEmpty())
            page_product_overview->setObjectName(QStringLiteral("page_product_overview"));
        page_product_overview->resize(1125, 1920);
        QFont font;
        font.setFamily(QStringLiteral("Montserrat"));
        font.setBold(false);
        font.setWeight(50);
        page_product_overview->setFont(font);
        page_product_overview->setStyleSheet(QLatin1String("color: #FFFFFF;\n"
"font-family: Montserrat;\n"
"font-style: light;\n"
"font-weight: normal;\n"
"font-size: 23px;\n"
"line-height: 29px;"));
        pushButton_previous_page = new QPushButton(page_product_overview);
        pushButton_previous_page->setObjectName(QStringLiteral("pushButton_previous_page"));
        pushButton_previous_page->setGeometry(QRect(70, 1757, 281, 100));
        pushButton_continue = new QPushButton(page_product_overview);
        pushButton_continue->setObjectName(QStringLiteral("pushButton_continue"));
        pushButton_continue->setGeometry(QRect(188, 1541, 740, 92));
        pushButton_continue->setFont(font);
        pushButton_continue->setAutoFillBackground(false);
        pushButton_continue->setStyleSheet(QStringLiteral(""));
        pushButton_to_help = new QPushButton(page_product_overview);
        pushButton_to_help->setObjectName(QStringLiteral("pushButton_to_help"));
        pushButton_to_help->setGeometry(QRect(446, 1727, 211, 191));
        label_invoice_price = new QLabel(page_product_overview);
        label_invoice_price->setObjectName(QStringLiteral("label_invoice_price"));
        label_invoice_price->setGeometry(QRect(749, 993, 211, 80));
        label_invoice_price->setStyleSheet(QLatin1String("font-family: Montserrat;\n"
"background-image: url(/home/df-admin/production/references/background.png);\n"
"font-style: normal;\n"
"font-weight: bold;\n"
"font-size: 24px;\n"
"line-height: 29px;\n"
"\n"
"color: #7F8080;"));
        label_invoice_price->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_invoice_price_total = new QLabel(page_product_overview);
        label_invoice_price_total->setObjectName(QStringLiteral("label_invoice_price_total"));
        label_invoice_price_total->setGeometry(QRect(659, 1340, 301, 44));
        label_invoice_price_total->setStyleSheet(QLatin1String("font-family: Montserrat;\n"
"background-image: url(background.png);\n"
"font-style: light;\n"
"font-weight: bold;\n"
"font-size: 36px;\n"
"line-height: 44px;\n"
"\n"
"color: #58595B;"));
        label_invoice_price_total->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_selected_volume = new QLabel(page_product_overview);
        label_selected_volume->setObjectName(QStringLiteral("label_selected_volume"));
        label_selected_volume->setGeometry(QRect(538, 256, 445, 450));
        QFont font1;
        font1.setFamily(QStringLiteral("Montserrat"));
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(50);
        label_selected_volume->setFont(font1);
        label_selected_volume->setLayoutDirection(Qt::LeftToRight);
        label_selected_volume->setStyleSheet(QLatin1String("font-family: Montserrat;\n"
"font-style: normal;\n"
"font-weight: normal;\n"
"font-size: 20px;\n"
"line-height: 40px;\n"
"color: #FFFFFF;\n"
"text-align: center;\n"
""));
        label_selected_volume->setAlignment(Qt::AlignCenter);
        label_selected_price = new QLabel(page_product_overview);
        label_selected_price->setObjectName(QStringLiteral("label_selected_price"));
        label_selected_price->setGeometry(QRect(668, 830, 317, 55));
        QFont font2;
        font2.setFamily(QStringLiteral("Montserrat"));
        font2.setBold(true);
        font2.setWeight(75);
        label_selected_price->setFont(font2);
        label_selected_price->setStyleSheet(QLatin1String("color: #FFFFFF;\n"
"font-family: Montserrat;\n"
"font-style: bold;\n"
"font-weight:bold;\n"
"font-size: 36px;\n"
"line-height: 24px;\n"
"text-align: center;"));
        label_selected_price->setAlignment(Qt::AlignCenter);
        lineEdit_promo_code = new QLineEdit(page_product_overview);
        lineEdit_promo_code->setObjectName(QStringLiteral("lineEdit_promo_code"));
        lineEdit_promo_code->setEnabled(true);
        lineEdit_promo_code->setGeometry(QRect(94, 775, 891, 46));
        lineEdit_promo_code->setMinimumSize(QSize(391, 35));
        lineEdit_promo_code->setAutoFillBackground(false);
        lineEdit_promo_code->setStyleSheet(QStringLiteral(""));
        lineEdit_promo_code->setFrame(false);
        pushButton_promo_input = new QPushButton(page_product_overview);
        pushButton_promo_input->setObjectName(QStringLiteral("pushButton_promo_input"));
        pushButton_promo_input->setGeometry(QRect(90, 770, 891, 46));
        pushButton_promo_input->setMinimumSize(QSize(391, 37));
        pushButton_promo_input->setAutoFillBackground(false);
        pushButton_promo_input->setStyleSheet(QStringLiteral(""));
        pushButton_promo_input->setFlat(true);
        promoKeyboard = new QWidget(page_product_overview);
        promoKeyboard->setObjectName(QStringLiteral("promoKeyboard"));
        promoKeyboard->setEnabled(true);
        promoKeyboard->setGeometry(QRect(120, 830, 841, 320));
        promoKeyboard->setMouseTracking(false);
        promoKeyboard->setTabletTracking(true);
        promoKeyboard->setAutoFillBackground(false);
        num1 = new QPushButton(promoKeyboard);
        buttonGroup = new QButtonGroup(page_product_overview);
        buttonGroup->setObjectName(QStringLiteral("buttonGroup"));
        buttonGroup->addButton(num1);
        num1->setObjectName(QStringLiteral("num1"));
        num1->setGeometry(QRect(53, 32, 62, 47));
        num1->setMinimumSize(QSize(62, 47));
        num1->setMaximumSize(QSize(62, 47));
        num1->setTabletTracking(false);
        num1->setFocusPolicy(Qt::NoFocus);
        num1->setAutoFillBackground(false);
        num1->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        num1->setFlat(true);
        num2 = new QPushButton(promoKeyboard);
        buttonGroup->addButton(num2);
        num2->setObjectName(QStringLiteral("num2"));
        num2->setGeometry(QRect(128, 32, 62, 47));
        num2->setMinimumSize(QSize(62, 47));
        num2->setMaximumSize(QSize(62, 47));
        num2->setFocusPolicy(Qt::NoFocus);
        num2->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        num2->setFlat(true);
        num3 = new QPushButton(promoKeyboard);
        buttonGroup->addButton(num3);
        num3->setObjectName(QStringLiteral("num3"));
        num3->setGeometry(QRect(203, 32, 62, 47));
        num3->setMinimumSize(QSize(62, 47));
        num3->setMaximumSize(QSize(62, 47));
        num3->setFocusPolicy(Qt::NoFocus);
        num3->setContextMenuPolicy(Qt::PreventContextMenu);
        num3->setAcceptDrops(false);
        num3->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        num3->setFlat(true);
        num4 = new QPushButton(promoKeyboard);
        buttonGroup->addButton(num4);
        num4->setObjectName(QStringLiteral("num4"));
        num4->setGeometry(QRect(278, 32, 62, 47));
        num4->setMinimumSize(QSize(62, 47));
        num4->setMaximumSize(QSize(62, 47));
        num4->setFocusPolicy(Qt::NoFocus);
        num4->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        num4->setFlat(true);
        num5 = new QPushButton(promoKeyboard);
        buttonGroup->addButton(num5);
        num5->setObjectName(QStringLiteral("num5"));
        num5->setGeometry(QRect(353, 32, 62, 47));
        num5->setMinimumSize(QSize(62, 47));
        num5->setMaximumSize(QSize(62, 47));
        num5->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        num5->setFlat(true);
        num6 = new QPushButton(promoKeyboard);
        buttonGroup->addButton(num6);
        num6->setObjectName(QStringLiteral("num6"));
        num6->setGeometry(QRect(428, 32, 62, 47));
        num6->setMinimumSize(QSize(62, 47));
        num6->setMaximumSize(QSize(62, 47));
        num6->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        num6->setFlat(true);
        num7 = new QPushButton(promoKeyboard);
        buttonGroup->addButton(num7);
        num7->setObjectName(QStringLiteral("num7"));
        num7->setGeometry(QRect(503, 32, 62, 47));
        num7->setMinimumSize(QSize(62, 47));
        num7->setMaximumSize(QSize(62, 47));
        num7->setFocusPolicy(Qt::ClickFocus);
        num7->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        num7->setFlat(true);
        num8 = new QPushButton(promoKeyboard);
        buttonGroup->addButton(num8);
        num8->setObjectName(QStringLiteral("num8"));
        num8->setGeometry(QRect(578, 32, 62, 47));
        num8->setMinimumSize(QSize(62, 47));
        num8->setMaximumSize(QSize(62, 47));
        num8->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        num8->setFlat(true);
        num9 = new QPushButton(promoKeyboard);
        buttonGroup->addButton(num9);
        num9->setObjectName(QStringLiteral("num9"));
        num9->setGeometry(QRect(653, 32, 62, 47));
        num9->setMinimumSize(QSize(62, 47));
        num9->setMaximumSize(QSize(62, 47));
        num9->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        num9->setFlat(true);
        num0 = new QPushButton(promoKeyboard);
        buttonGroup->addButton(num0);
        num0->setObjectName(QStringLiteral("num0"));
        num0->setGeometry(QRect(728, 32, 62, 47));
        num0->setMinimumSize(QSize(62, 47));
        num0->setMaximumSize(QSize(62, 47));
        num0->setAutoFillBackground(false);
        num0->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        num0->setFlat(true);
        q = new QPushButton(promoKeyboard);
        buttonGroup->addButton(q);
        q->setObjectName(QStringLiteral("q"));
        q->setGeometry(QRect(53, 89, 62, 62));
        q->setMinimumSize(QSize(62, 62));
        q->setMaximumSize(QSize(62, 62));
        q->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        q->setFlat(true);
        w = new QPushButton(promoKeyboard);
        buttonGroup->addButton(w);
        w->setObjectName(QStringLiteral("w"));
        w->setGeometry(QRect(128, 89, 62, 62));
        w->setMinimumSize(QSize(62, 62));
        w->setMaximumSize(QSize(62, 62));
        w->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        w->setFlat(true);
        e = new QPushButton(promoKeyboard);
        buttonGroup->addButton(e);
        e->setObjectName(QStringLiteral("e"));
        e->setGeometry(QRect(203, 89, 62, 62));
        e->setMinimumSize(QSize(62, 62));
        e->setMaximumSize(QSize(62, 62));
        e->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        e->setFlat(true);
        r = new QPushButton(promoKeyboard);
        buttonGroup->addButton(r);
        r->setObjectName(QStringLiteral("r"));
        r->setGeometry(QRect(278, 89, 62, 62));
        r->setMinimumSize(QSize(62, 62));
        r->setMaximumSize(QSize(62, 62));
        r->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        r->setFlat(true);
        t = new QPushButton(promoKeyboard);
        buttonGroup->addButton(t);
        t->setObjectName(QStringLiteral("t"));
        t->setGeometry(QRect(353, 89, 62, 62));
        t->setMinimumSize(QSize(62, 62));
        t->setMaximumSize(QSize(62, 62));
        t->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        t->setFlat(true);
        y = new QPushButton(promoKeyboard);
        buttonGroup->addButton(y);
        y->setObjectName(QStringLiteral("y"));
        y->setGeometry(QRect(428, 89, 62, 62));
        y->setMinimumSize(QSize(62, 62));
        y->setMaximumSize(QSize(62, 62));
        y->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        y->setFlat(true);
        u = new QPushButton(promoKeyboard);
        buttonGroup->addButton(u);
        u->setObjectName(QStringLiteral("u"));
        u->setGeometry(QRect(503, 89, 62, 62));
        u->setMinimumSize(QSize(62, 62));
        u->setMaximumSize(QSize(62, 62));
        u->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        u->setFlat(true);
        i = new QPushButton(promoKeyboard);
        buttonGroup->addButton(i);
        i->setObjectName(QStringLiteral("i"));
        i->setGeometry(QRect(578, 89, 62, 62));
        i->setMinimumSize(QSize(62, 62));
        i->setMaximumSize(QSize(62, 62));
        i->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        i->setFlat(true);
        o = new QPushButton(promoKeyboard);
        buttonGroup->addButton(o);
        o->setObjectName(QStringLiteral("o"));
        o->setGeometry(QRect(653, 89, 62, 62));
        o->setMinimumSize(QSize(62, 62));
        o->setMaximumSize(QSize(62, 62));
        o->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        o->setFlat(true);
        p = new QPushButton(promoKeyboard);
        buttonGroup->addButton(p);
        p->setObjectName(QStringLiteral("p"));
        p->setGeometry(QRect(728, 89, 62, 62));
        p->setMinimumSize(QSize(62, 62));
        p->setMaximumSize(QSize(62, 62));
        p->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        p->setFlat(true);
        a = new QPushButton(promoKeyboard);
        buttonGroup->addButton(a);
        a->setObjectName(QStringLiteral("a"));
        a->setGeometry(QRect(53, 161, 62, 62));
        a->setMinimumSize(QSize(62, 62));
        a->setMaximumSize(QSize(62, 62));
        a->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        a->setFlat(true);
        s = new QPushButton(promoKeyboard);
        buttonGroup->addButton(s);
        s->setObjectName(QStringLiteral("s"));
        s->setGeometry(QRect(128, 161, 62, 62));
        s->setMinimumSize(QSize(62, 62));
        s->setMaximumSize(QSize(62, 62));
        s->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        s->setFlat(true);
        d = new QPushButton(promoKeyboard);
        buttonGroup->addButton(d);
        d->setObjectName(QStringLiteral("d"));
        d->setGeometry(QRect(203, 161, 62, 62));
        d->setMinimumSize(QSize(62, 62));
        d->setMaximumSize(QSize(62, 62));
        d->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        d->setFlat(true);
        g = new QPushButton(promoKeyboard);
        buttonGroup->addButton(g);
        g->setObjectName(QStringLiteral("g"));
        g->setGeometry(QRect(353, 161, 62, 62));
        g->setMinimumSize(QSize(62, 62));
        g->setMaximumSize(QSize(62, 62));
        g->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        g->setFlat(true);
        f = new QPushButton(promoKeyboard);
        buttonGroup->addButton(f);
        f->setObjectName(QStringLiteral("f"));
        f->setGeometry(QRect(278, 161, 62, 62));
        f->setMinimumSize(QSize(62, 62));
        f->setMaximumSize(QSize(62, 62));
        f->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        f->setFlat(true);
        h = new QPushButton(promoKeyboard);
        buttonGroup->addButton(h);
        h->setObjectName(QStringLiteral("h"));
        h->setGeometry(QRect(428, 161, 62, 62));
        h->setMinimumSize(QSize(62, 62));
        h->setMaximumSize(QSize(62, 62));
        h->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        h->setFlat(true);
        j = new QPushButton(promoKeyboard);
        buttonGroup->addButton(j);
        j->setObjectName(QStringLiteral("j"));
        j->setGeometry(QRect(503, 161, 62, 62));
        j->setMinimumSize(QSize(62, 62));
        j->setMaximumSize(QSize(62, 62));
        j->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        j->setFlat(true);
        k = new QPushButton(promoKeyboard);
        buttonGroup->addButton(k);
        k->setObjectName(QStringLiteral("k"));
        k->setGeometry(QRect(578, 161, 62, 62));
        k->setMinimumSize(QSize(62, 62));
        k->setMaximumSize(QSize(62, 62));
        k->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        k->setFlat(true);
        l = new QPushButton(promoKeyboard);
        buttonGroup->addButton(l);
        l->setObjectName(QStringLiteral("l"));
        l->setGeometry(QRect(653, 161, 62, 62));
        l->setMinimumSize(QSize(62, 62));
        l->setMaximumSize(QSize(62, 62));
        l->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        l->setFlat(true);
        z = new QPushButton(promoKeyboard);
        buttonGroup->addButton(z);
        z->setObjectName(QStringLiteral("z"));
        z->setGeometry(QRect(103, 234, 62, 62));
        z->setMinimumSize(QSize(62, 62));
        z->setMaximumSize(QSize(62, 62));
        z->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        z->setFlat(true);
        x = new QPushButton(promoKeyboard);
        buttonGroup->addButton(x);
        x->setObjectName(QStringLiteral("x"));
        x->setGeometry(QRect(178, 234, 62, 62));
        x->setMinimumSize(QSize(62, 62));
        x->setMaximumSize(QSize(62, 62));
        x->setFocusPolicy(Qt::NoFocus);
        x->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        x->setFlat(true);
        c = new QPushButton(promoKeyboard);
        buttonGroup->addButton(c);
        c->setObjectName(QStringLiteral("c"));
        c->setGeometry(QRect(253, 234, 62, 62));
        c->setMinimumSize(QSize(62, 62));
        c->setMaximumSize(QSize(62, 62));
        c->setFocusPolicy(Qt::NoFocus);
        c->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        c->setFlat(true);
        v = new QPushButton(promoKeyboard);
        buttonGroup->addButton(v);
        v->setObjectName(QStringLiteral("v"));
        v->setGeometry(QRect(328, 234, 62, 62));
        v->setMinimumSize(QSize(62, 62));
        v->setMaximumSize(QSize(62, 62));
        v->setFocusPolicy(Qt::NoFocus);
        v->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        v->setFlat(true);
        b = new QPushButton(promoKeyboard);
        buttonGroup->addButton(b);
        b->setObjectName(QStringLiteral("b"));
        b->setGeometry(QRect(403, 234, 62, 62));
        b->setMinimumSize(QSize(62, 62));
        b->setMaximumSize(QSize(62, 62));
        b->setFocusPolicy(Qt::NoFocus);
        b->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        b->setFlat(true);
        n = new QPushButton(promoKeyboard);
        buttonGroup->addButton(n);
        n->setObjectName(QStringLiteral("n"));
        n->setGeometry(QRect(478, 234, 62, 62));
        n->setMinimumSize(QSize(62, 62));
        n->setMaximumSize(QSize(62, 62));
        n->setFocusPolicy(Qt::NoFocus);
        n->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        n->setFlat(true);
        m = new QPushButton(promoKeyboard);
        buttonGroup->addButton(m);
        m->setObjectName(QStringLiteral("m"));
        m->setGeometry(QRect(553, 234, 62, 62));
        m->setMinimumSize(QSize(62, 62));
        m->setMaximumSize(QSize(62, 62));
        m->setFocusPolicy(Qt::NoFocus);
        m->setAutoFillBackground(false);
        m->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        m->setFlat(true);
        backspace = new QPushButton(promoKeyboard);
        buttonGroup->addButton(backspace);
        backspace->setObjectName(QStringLiteral("backspace"));
        backspace->setGeometry(QRect(728, 161, 77, 62));
        backspace->setMaximumSize(QSize(87, 62));
        backspace->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        backspace->setFlat(true);
        done = new QPushButton(promoKeyboard);
        buttonGroup->addButton(done);
        done->setObjectName(QStringLiteral("done"));
        done->setEnabled(true);
        done->setGeometry(QRect(628, 234, 125, 64));
        done->setMaximumSize(QSize(125, 64));
        done->setFocusPolicy(Qt::ClickFocus);
        done->setAutoFillBackground(false);
        done->setStyleSheet(QLatin1String("background:transparent;\n"
"background-color:rgba(238, 238, 236,0);\n"
"border:none;\n"
"outline: none;\n"
""));
        done->setFlat(true);
        label_keyboard_background = new QLabel(promoKeyboard);
        label_keyboard_background->setObjectName(QStringLiteral("label_keyboard_background"));
        label_keyboard_background->setGeometry(QRect(0, 0, 841, 321));
        num0->raise();
        num2->raise();
        done->raise();
        h->raise();
        t->raise();
        num7->raise();
        j->raise();
        num4->raise();
        z->raise();
        g->raise();
        o->raise();
        p->raise();
        y->raise();
        q->raise();
        n->raise();
        num9->raise();
        v->raise();
        b->raise();
        c->raise();
        m->raise();
        backspace->raise();
        num3->raise();
        num8->raise();
        d->raise();
        x->raise();
        i->raise();
        e->raise();
        k->raise();
        num5->raise();
        r->raise();
        num6->raise();
        f->raise();
        s->raise();
        u->raise();
        a->raise();
        l->raise();
        w->raise();
        num1->raise();
        label_keyboard_background->raise();
        label_invoice_name = new QLabel(page_product_overview);
        label_invoice_name->setObjectName(QStringLiteral("label_invoice_name"));
        label_invoice_name->setGeometry(QRect(130, 993, 590, 90));
        label_invoice_name->setStyleSheet(QLatin1String("position: absolute;\n"
"height: 29px;\n"
"left: 17.31%;\n"
"right: 43.8%;\n"
"top: calc(50% - 29px/2 + 389.5px);\n"
"\n"
"font-family: Montserrat;\n"
"font-style: normal;\n"
"font-weight: normal;\n"
"font-size: 24px;\n"
"line-height: 29px;\n"
"\n"
"color: #58595B;\n"
"\n"
""));
        label_invoice_discount_amount = new QLabel(page_product_overview);
        label_invoice_discount_amount->setObjectName(QStringLiteral("label_invoice_discount_amount"));
        label_invoice_discount_amount->setEnabled(true);
        label_invoice_discount_amount->setGeometry(QRect(751, 1160, 201, 80));
        label_invoice_discount_amount->setStyleSheet(QLatin1String("font-family: Montserrat;\n"
"background-image: url(/home/df-admin/production/references/background.png);\n"
"font-style: normal;\n"
"font-weight: normal;\n"
"font-size: 24px;\n"
"line-height: 29px;\n"
"\n"
"color: #7F8080;"));
        label_invoice_discount_amount->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_invoice_discount_name = new QLabel(page_product_overview);
        label_invoice_discount_name->setObjectName(QStringLiteral("label_invoice_discount_name"));
        label_invoice_discount_name->setEnabled(true);
        label_invoice_discount_name->setGeometry(QRect(130, 1400, 641, 41));
        label_invoice_discount_name->setStyleSheet(QLatin1String("position: absolute;\n"
"height: 29px;\n"
"left: 17.31%;\n"
"right: 43.8%;\n"
"top: calc(50% - 29px/2 + 389.5px);\n"
"\n"
"font-family: Montserrat;\n"
"font-style: normal;\n"
"font-weight: normal;\n"
"font-size: 24px;\n"
"line-height: 29px;\n"
"\n"
"color: #58595B;\n"
"\n"
""));
        label_product_photo = new QLabel(page_product_overview);
        label_product_photo->setObjectName(QStringLiteral("label_product_photo"));
        label_product_photo->setGeometry(QRect(94, 256, 445, 450));
        label_product_photo->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_discount_tag = new QLabel(page_product_overview);
        label_discount_tag->setObjectName(QStringLiteral("label_discount_tag"));
        label_discount_tag->setEnabled(true);
        label_discount_tag->setGeometry(QRect(130, 1175, 185, 34));
        label_discount_tag->setLayoutDirection(Qt::LeftToRight);
        label_discount_tag->setStyleSheet(QStringLiteral(""));
        label_discount_tag->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        pushButton_select_product_page = new QPushButton(page_product_overview);
        pushButton_select_product_page->setObjectName(QStringLiteral("pushButton_select_product_page"));
        pushButton_select_product_page->setGeometry(QRect(70, 40, 401, 41));
        label_invoice_box = new QLabel(page_product_overview);
        label_invoice_box->setObjectName(QStringLiteral("label_invoice_box"));
        label_invoice_box->setGeometry(QRect(94, 890, 891, 760));
        label_invoice_box->setFont(font2);
        label_invoice_box->setStyleSheet(QLatin1String("color: #FFFFFF;\n"
"font-family: Montserrat;\n"
"font-style: bold;\n"
"font-weight:bold;\n"
"font-size: 36px;\n"
"line-height: 24px;\n"
"text-align: center;"));
        label_invoice_box->setAlignment(Qt::AlignCenter);
        label_gif = new QLabel(page_product_overview);
        label_gif->setObjectName(QStringLiteral("label_gif"));
        label_gif->setGeometry(QRect(440, 960, 200, 200));
        label_gif->setAutoFillBackground(false);
        label_total = new QLabel(page_product_overview);
        label_total->setObjectName(QStringLiteral("label_total"));
        label_total->setGeometry(QRect(130, 1340, 641, 44));
        label_help = new QLabel(page_product_overview);
        label_help->setObjectName(QStringLiteral("label_help"));
        label_help->setGeometry(QRect(478, 1773, 125, 125));
        line_invoice = new QFrame(page_product_overview);
        line_invoice->setObjectName(QStringLiteral("line_invoice"));
        line_invoice->setGeometry(QRect(137, 1280, 821, 5));
        line_invoice->setFrameShape(QFrame::HLine);
        line_invoice->setFrameShadow(QFrame::Sunken);
        label_help->raise();
        pushButton_continue->raise();
        label_invoice_box->raise();
        label_invoice_price->raise();
        lineEdit_promo_code->raise();
        pushButton_previous_page->raise();
        label_invoice_price_total->raise();
        label_invoice_name->raise();
        label_invoice_discount_amount->raise();
        label_selected_volume->raise();
        label_selected_price->raise();
        label_invoice_discount_name->raise();
        label_discount_tag->raise();
        pushButton_select_product_page->raise();
        pushButton_promo_input->raise();
        label_total->raise();
        promoKeyboard->raise();
        label_product_photo->raise();
        label_gif->raise();
        pushButton_to_help->raise();
        line_invoice->raise();

        retranslateUi(page_product_overview);

        QMetaObject::connectSlotsByName(page_product_overview);
    } // setupUi

    void retranslateUi(QWidget *page_product_overview)
    {
        page_product_overview->setWindowTitle(QApplication::translate("page_product_overview", "Form", Q_NULLPTR));
        pushButton_previous_page->setText(QString());
        pushButton_continue->setText(QString());
        pushButton_to_help->setText(QString());
        label_invoice_price->setText(QString());
        label_invoice_price_total->setText(QString());
        label_selected_volume->setText(QString());
        label_selected_price->setText(QString());
        lineEdit_promo_code->setText(QString());
        pushButton_promo_input->setText(QString());
        num1->setText(QString());
        num2->setText(QString());
        num3->setText(QString());
        num4->setText(QString());
        num5->setText(QString());
        num6->setText(QString());
        num7->setText(QString());
        num8->setText(QString());
        num9->setText(QString());
        num0->setText(QString());
        q->setText(QString());
        w->setText(QString());
        e->setText(QString());
        r->setText(QString());
        t->setText(QString());
        y->setText(QString());
        u->setText(QString());
        i->setText(QString());
        o->setText(QString());
        p->setText(QString());
        a->setText(QString());
        s->setText(QString());
        d->setText(QString());
        g->setText(QString());
        f->setText(QString());
        h->setText(QString());
        j->setText(QString());
        k->setText(QString());
        l->setText(QString());
        z->setText(QString());
        x->setText(QString());
        c->setText(QString());
        v->setText(QString());
        b->setText(QString());
        n->setText(QString());
        m->setText(QString());
        backspace->setText(QString());
        done->setText(QString());
        label_keyboard_background->setText(QApplication::translate("page_product_overview", "TextLabel", Q_NULLPTR));
        label_invoice_name->setText(QString());
        label_invoice_discount_amount->setText(QApplication::translate("page_product_overview", "$ 0.00", Q_NULLPTR));
        label_invoice_discount_name->setText(QString());
        label_product_photo->setText(QString());
        label_discount_tag->setText(QString());
        pushButton_select_product_page->setText(QApplication::translate("page_product_overview", "PushButton", Q_NULLPTR));
        label_invoice_box->setText(QString());
        label_gif->setText(QString());
        label_total->setText(QString());
        label_help->setText(QApplication::translate("page_product_overview", "TextLabel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class page_product_overview: public Ui_page_product_overview {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_PRODUCTOVERVIEW_H
