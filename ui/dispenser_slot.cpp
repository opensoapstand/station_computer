#include "df_util.h" // lode added for settings
#include "dbmanager.h"
// #include "page_dispenser.h"
#include "page_idle.h"
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

int dispenser_slot::getBasePNumber()
{
    return m_basePNumber;
}

QVector<int> dispenser_slot::getAllPNumbers()
{

    QVector<int> pnumbers(m_dispensePNumbers);

    // Copy elements from m_additivePNumbers
    for (int i = 0; i < m_additivePNumbers.size(); ++i)
    {
        pnumbers.append(m_additivePNumbers[i]);
    }

    pnumbers.append(m_basePNumber);
    return pnumbers;
}

void dispenser_slot::loadSlotParametersFromDb()
{
    m_db->getAllSlotProperties(getSlotId(),
                               &m_dispensePNumbers,
                               &m_basePNumber,
                               &m_additivePNumbers,
                               &m_is_enabled,
                               &m_status_text);
}

// void dispenser_slot::setSlotEnabled(bool isEnabled)
// {
//     thisMachine->setSlotEnabled(getSlotId(), isEnabled);
// }

// void dispenser_slot::setSlotEnabled(bool isEnabled, QString statusText)
// {
//     thisMachine->setSlotEnabled(getSlotId(), isEnabled);
//     setStatusText(statusText);
// }

// bool dispenser_slot::getSlotEnabled()
// {
//     return thisMachine->getSlotEnabled(getSlotId());
// }

// void dispenser_slot::setStatusText(QString status)
// {
//     thisMachine->setStatusText(getSlotId(), getSlotEnabled(), status);
// }

// QString dispenser_slot::getStatusText()
// {
//     return thisMachine->getStatusText(getSlotId());
// }

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
