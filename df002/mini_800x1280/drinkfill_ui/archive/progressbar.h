#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>

class ProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressBar(QWidget *parent = nullptr);
    void paintEvent( QPaintEvent* ) override;

signals:

public slots:
};

#endif // PROGRESSBAR_H
