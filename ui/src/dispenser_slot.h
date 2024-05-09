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

    int getBasePNumber();

    int getDispenseProductsCount();
    bool isOnlyOneProductAvailable();
    int getDispensePNumber(int position);
    QVector<int> getDispensePNumbers();

    int getAdditivePNumber(int position);
    int getAdditiveCount();
    QVector<int> getAdditivePNumbers();

    bool loadSlotParametersFromDb();

    // Setters and Getters
    void setSlot(int slot);
    int getSlotId();

    void setSlotEnabled(bool isEnabled, SlotState status);
    void setSlotEnabled(bool isEnabled);
    bool getIsSlotEnabled();

    SlotState getSlotStatus();
    void setSlotStatus(SlotState state);
    void setSlotStatusText(QString status);
    QString getSlotStatusAsString();

    QString getDispenseStartTime();
    QString getDispenseEndTime();
    double getButtonPressDuration();
    double getButtonPressCount();
    void setDispenseStartTime(QString start_time);
    void setDispenseEndTime(QString end_time);
    void setButtonPressDuration(double button_duration);
    void setButtonPressCount(double button_count);
    void setDispenseBehaviour(QString dispenseBehaviourString);
    void setDispenseBehaviour(DispenseBehaviour);
    DispenseBehaviour getDispenseBehaviour();
    QString getDispenseBehaviourAsString();

    DispenseBehaviour m_dispense_behaviour;
    // void setBasePNumber(int);
    // void getBasePNumber(int);
    // int *getAdditivesPNumbers();

    // void setCouponCode(QString promoCode);
    // bool getSlotEnabled();
    // void setSlotEnabled(bool isEnabled);
    // void setSlotEnabled(bool isEnabled, QString statusText);

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
    QVector<int> m_dispensePNumbers;
    QVector<int> m_additivePNumbers; // int m_mixPNumbers[ADDITIVES_PER_SLOT_COUNT_MAX];
    bool m_is_enabled;
    QString m_status_text;
    SlotState m_slot_state;
    QString m_dispense_start_time;
    QString m_dispense_end_time;
    double m_button_press_duration;
    double m_button_press_count;

    // QString m_paymentOptions;  // --> machine?!
    // QString m_currency; // --> machine

    int m_dispenser_slot;
    // double overruledPrice;

    QString m_promoCode;
};

#endif // PRODUCT_H
