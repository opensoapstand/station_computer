#include "df_util.h" // lode added for settings
#include "dbmanager.h"
// #include "page_dispenser.h"
#include "dispenser_slot.h"
#include "machine.h"

// Ctor
dispenser_slot::dispenser_slot()
{

    m_promoCode = "";
}

// Ctor Object Copy
dispenser_slot::dispenser_slot(const dispenser_slot &other) : QObject(nullptr)
{
}

// Dtor
dispenser_slot::~dispenser_slot()
{
}

void dispenser_slot::setDb(DbManager *db)
{
    m_db = db;
}

// void dispenser_slot::setMachine(machine *machine)
// {
//     thisMachine = machine;
// }
QVector<int> dispenser_slot::getDispensePNumbers()
{
    return m_dispensePNumbers;
}
QVector<int> dispenser_slot::getAdditivePNumbers()
{
    return m_additivePNumbers;
}
int dispenser_slot::getDispensePNumber(int position)
{
  if (position == 0)
    {
        qDebug() << "Dispense product Position cannot be 0 , must start from 1.";
        return DUMMY_PNUMBER;
    }
    if (position > getDispenseProductsCount())
    {
        qDebug() << "Dispense Product not existing.";
        return DUMMY_PNUMBER;
    }
    return m_dispensePNumbers[position - 1];
}

int dispenser_slot::getAdditivePNumber(int position)
{
    // position starts from 1
    if (position == 0)
    {
        qDebug() << "Additive Position cannot be 0 , must start from 1.";
        return DUMMY_PNUMBER;
    }
    if (position > getAdditiveCount())
    {
        qDebug() << "Additive not existing.";
        return DUMMY_PNUMBER;
    }
    return m_additivePNumbers[position - 1];
}

int dispenser_slot::getAdditiveCount()
{
    return m_additivePNumbers.size();
}

int dispenser_slot::getDispenseProductsCount()
{
    return m_dispensePNumbers.size();
}

int dispenser_slot::getBasePNumber()
{
    return m_basePNumber;
}

QVector<int> dispenser_slot::getAllPNumbers()
{

    QSet<int> pnumbers; // Use a QSet to store unique pnumbers (i.e. no value can appear twice)

    for (const int &value : m_dispensePNumbers)
    {
        pnumbers.insert(value);
    }

    // QVector<int> pnumbers(m_dispensePNumbers);
    qDebug() << "dispensePNumbers Count: " << m_dispensePNumbers.size();
    qDebug() << "dispensePNumbers Count: " << pnumbers.size();

    // Copy elements from m_additivePNumbers
    for (int i = 0; i < m_additivePNumbers.size(); ++i)
    {

        pnumbers.insert(m_additivePNumbers[i]);
    }
    qDebug() << "m_additivePNumbers Count: " << m_additivePNumbers.size();

    pnumbers.insert(m_basePNumber);
    qDebug() << "base pnumber value: " << m_basePNumber;

    int option = 1;
    foreach (const int &value, pnumbers)
    {
        if (value < 0)
        {
            qDebug() << "Assert error: pNumber invalid too low (" << value << " ) at option: " << option;
        }
        if (value > HIGHEST_PNUMBER_COUNT)
        {
            qDebug() << "Assert error: pNumber invalid too high(" << value << " ) at option: " << option;
        }
        // std::cout << value << std::endl;
        option++;
    }

    // qDebug()<<"size of all pnumbers: " <<pnumbers.size();

    // return pnumbers;
    return QVector<int>::fromList(pnumbers.toList());
}

void dispenser_slot::loadSlotParametersFromDb()
{
    m_db->getAllSlotProperties(getSlotId(),
                               m_dispensePNumbers,
                               m_basePNumber,
                               m_additivePNumbers,
                               m_is_enabled,
                               m_status_text);
}

void dispenser_slot::setSlotEnabled(bool isEnabled, QString statusText)
{
    setSlotEnabled(isEnabled);
    setStatusText(statusText);
}

void dispenser_slot::setSlotEnabled(bool isEnabled)
{
    // thisMachine->setSlotEnabled(getSlotId(), isEnabled);
    m_is_enabled = isEnabled;
    m_db->updateTableSlotsWithInt(getSlotId(), "is_enabled", m_is_enabled);
}

bool dispenser_slot::getIsSlotEnabled()
{
    return m_is_enabled;
}

void dispenser_slot::setStatusText(QString status)
{
    m_status_text = status;
    m_db->updateTableSlotsWithText(getSlotId(), "status_text", m_status_text);
}

QString dispenser_slot::getStatusText()
{
    return m_status_text;
    // return thisMachine->getStatusText(getSlotId());
}

int dispenser_slot::getSlotId()
{
    return m_dispenser_slot;
}

void dispenser_slot::setSlot(int slot)
{
    if (slot == 0)
    {
        qDebug() << "ERROR: 0 provided.  Slots start counting from 1. Not zero.";
    }
    if (slot > MAX_SLOT_COUNT)
    {
        qDebug() << "ERROR: Slot exceeds max slot count." << slot;
    }
    m_dispenser_slot = slot;
}

void dispenser_slot::setDispenseStartTime(QString start_time){
    m_dispense_start_time = start_time;
}

void dispenser_slot::setDispenseEndTime(QString end_time){
    m_dispense_end_time = end_time;
}

QString dispenser_slot::getDispenseStartTime(){
    return m_dispense_start_time;
}

QString dispenser_slot::getDispenseEndTime(){
    return m_dispense_end_time;
}

void dispenser_slot::setButtonPressDuration(double button_duration){
    m_button_press_duration = button_duration;
}

void dispenser_slot::setButtonPressCount(double button_count){
    m_button_press_count = button_count;
}

double dispenser_slot::getButtonPressDuration(){
    return m_button_press_duration;
}

double dispenser_slot::getButtonPressCount(){
    return m_button_press_count;
}