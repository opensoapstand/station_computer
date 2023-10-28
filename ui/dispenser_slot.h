#ifndef PRODUCT_H
#define PRODUCT_H

#include "df_util.h"
#include "machine.h"
#include "dbmanager.h"

class machine; // forward declaration.

class dispenser_slot : public QObject
{
    Q_OBJECT

public:
    dispenser_slot();
    dispenser_slot(const dispenser_slot &other);
    ~dispenser_slot();
    dispenser_slot &operator=(const dispenser_slot &other);
    // void setMachine(machine *machine);
    void setDb(DbManager *db);

    QVector<int> getAllPNumbers();

    void loadSlotParametersFromDb();

    // Setters and Getters
    void setSlot(int slot);
    int getSlotId();

    // void setBasePNumber(int);
    void getBasePNumber(int);
    int getAdditivesPNumbersCount();
    int *getAdditivesPNumbers();

    // void setCouponCode(QString promoCode);
    // bool getSlotEnabled();
    // void setSlotEnabled(bool isEnabled);
    // void setSlotEnabled(bool isEnabled, QString statusText);
    // QString getStatusText();
    // void setStatusText(QString status);

public slots:

signals:
    // void orderSlotChange(int newOpt);
    // void priceChange(double newPrice);
    // void sizeChange(double newSize);

private:
    machine *thisMachine;
    DbManager *m_db;

    int slot;

    int m_basePNumber;
    QVector<int> m_additivesPNumbers; // int m_additivesPNumbers[ADDITIVES_PER_SLOT_COUNT_MAX];
    bool m_is_enabled;
    QString m_status_text;

    // QString m_payment;  // --> machine?!
    // QString m_currency; // --> machine

    int m_dispenser_slot;
    // double overruledPrice;

    QString m_promoCode;
};

#endif // PRODUCT_H
