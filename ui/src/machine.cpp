#include "df_util.h" // lode added for settings
#include "dbmanager.h"
#include "machine.h"
#include "dispenser_slot.h"
#include "pnumberproduct.h"

#include <map>
#include <fstream>

#include <QtWidgets>
#include <QVector>
#include <QSet>

machine::machine()
{
    setRole(UserRole::user);
    setCouponState(no_state);

    // IPC Networking
    dfUtility = new df_util();
    dispenseProductsMenuOptions.resize(MENU_PRODUCT_SELECTION_OPTIONS_MAX);
    dispenseProductsMenuOptions.fill(DUMMY_PNUMBER);

    // initializing array, containing all existing pnumbers
    for (int pnumber_index = 0; pnumber_index < HIGHEST_PNUMBER_COUNT; pnumber_index++)
    {
        m_pnumberproducts[pnumber_index].setPNumber(pnumber_index);
    }
}

// Dtor
machine::~machine()
{
}

void machine::initMachine()
{
    loadMachineParameterFromDb(); // load here because we need parameters already at init

    // ASSUMES that there is always slot_id's starting from 1 to the set slot count.
    for (int slot_index = 0; slot_index < getSlotCount(); slot_index++)
    {
        m_slots[slot_index].setSlot(slot_index + 1);
        m_slots[slot_index].setDb(m_db);
        m_slots[slot_index].loadSlotParametersFromDb();
        // m_slots[slot_index].setEmptyContainerDetectionEnabled(getEmptyContainerDetectionEnabled());
    }

    // QVector<int> all_dispense_pnumbers = getAllUniqueDispensePNumbers();
    // initProductOptions(getAllUniqueDispensePNumbers());
    initProductOptions();

    // TODO: For now, the index of a product in an array is its P-number.  e.g. P-6 is tangerine dish saop. Its object resides in m_pnumberproducts[6], that will only work for less than a million something p-numbers...

    // load properties of all used pnumbers
    QVector<int> all_pnumbers = getAllUsedPNumbersFromSlots();
    for (int pnumber_index = 0; pnumber_index < all_pnumbers.size(); pnumber_index++)
    {
        m_pnumberproducts[all_pnumbers[pnumber_index]].setDb(m_db);
    }

    loadDynamicContent(); // part of it is redundant of what's been done here, but not everything. So, do it again.
}

bool machine::loadDynamicContent()
{
    // load global machine data
    bool success = true;
    success &= loadMachineParameterFromDb();

    m_slots_loaded_successfully = true;
    for (int slot_index = 0; slot_index < getSlotCount(); slot_index++)
    {
        m_slots_loaded_successfully &= m_slots[slot_index].loadSlotParametersFromDb();
    }
    success &= m_slots_loaded_successfully;

    // load properties of all used pnumbers
    QVector<int> all_pnumbers = getAllUsedPNumbersFromSlots();

    m_products_loaded_successfully = true;
    for (int pnumber_index = 0; pnumber_index < all_pnumbers.size(); pnumber_index++)
    {
        // qDebug() << "machine: load product properties for pnumber:" << (all_pnumbers[pnumber_index]);
        m_products_loaded_successfully &= m_pnumberproducts[all_pnumbers[pnumber_index]].loadProductProperties();
        m_pnumberproducts[all_pnumbers[pnumber_index]].setSizeUnit(getSizeUnit()); // volumeUnit is a machine wide parameter
    }

    success &= m_products_loaded_successfully;

    loadBottle();
    loadTextsFromTemplateCsv();                                // dynamic content (text by template)
    loadTextsFromDefaultHardwareCsv();                         // dynamic styling (css by template)
    loadTextsFromDefaultCsv();                                 // dynamic styling (css by template)
    loadElementDynamicPropertiesFromTemplate();                // dynamic elements (position, visibility)
    loadElementDynamicPropertiesFromDefaultHardwareTemplate(); // dynamic elements (position, visibility)
    loadElementDynamicPropertiesFromDefaultTemplate();         // dynamic elements (position, visibility)

    return success;
}

void machine::reboot()
{
    qDebug() << "Will reboot machine now.";
    QString command = "echo 'D@nkF1ll$' | sudo -S shutdown -r 0";
    system(qPrintable(command));
}

void machine::loadBottle()
{
    // default is pnumber 0;
    if (m_pNumber_bottle_1 != DUMMY_PNUMBER)
    {
        getProductByPNumber(m_pNumber_bottle_1)->loadProductProperties();
        getProductByPNumber(m_pNumber_bottle_1)->setSizeUnit(getSizeUnit());
    }
    if (m_pNumber_bottle_2)
    {
        getProductByPNumber(m_pNumber_bottle_2)->loadProductProperties();
        getProductByPNumber(m_pNumber_bottle_2)->setSizeUnit(getSizeUnit());
    }
}

QVector<int> machine::getAllDispensePNumbersFromSlot(int slot)
{
    int slot_index = slot - 1;
    return m_slots[slot_index].getDispensePNumbers();
}

QVector<int> machine::getAllUniqueDispensePNumbers()
{
    QSet<int> uniquePNumbers; // Use a QSet to store unique pnumbers (i.e. no value can appear twice)
    // dispense pnumber are the numbers used to be displayed in UI. The 'end' product. ()
    for (int slot_index = 0; slot_index < getSlotCount(); slot_index++)
    {
        QVector<int> slotpnumbers = getAllDispensePNumbersFromSlot(slot_index + 1);

        // Add unique pnumbers from the current slot to the QSet
        for (int i = 0; i < slotpnumbers.size(); ++i)
        {
            qDebug() << "Dispense product at slot " << (slot_index + 1) << ": pnumber: " << slotpnumbers[i];
            uniquePNumbers.insert(slotpnumbers[i]);
        }
    }
    // Convert the QSet to a QVector
    return QVector<int>::fromList(uniquePNumbers.toList());
}

QVector<int> machine::getAllUsedPNumbersFromSlots()
{
    // all pnumbers are all used dispense, base, addive numbers

    QSet<int> uniquePNumbers; // Use a QSet to store unique pnumbers (i.e. no value can appear twice)

    for (int slot_index = 0; slot_index < getSlotCount(); slot_index++)
    {
        QVector<int> slotpnumbers = m_slots[slot_index].getAllPNumbers();
        // Add unique pnumbers from the current slot to the QSet
        for (int i = 0; i < slotpnumbers.size(); ++i)
        {
            // qDebug() << "flbijb" << slotpnumbers[i];
            uniquePNumbers.insert(slotpnumbers[i]);
        }
    }

    // Convert the QSet to a QVector
    return QVector<int>::fromList(uniquePNumbers.toList());
}

bool machine::isProductVolumeInContainer(int pnumber)
{
    bool retval = true;

    if (getEmptyContainerDetectionEnabled())
    {
        retval = getProductByPNumber(pnumber)->getVolumeRemaining() > CONTAINER_EMPTY_THRESHOLD_ML;
    }
    return retval;
}

DbManager *machine::getDb()
{
    return m_db;
}

void machine::setDb(DbManager *db)
{
    m_db = db;
}

void machine::initProductOptions()
{
    dispenseProductsMenuOptions.resize(MENU_PRODUCT_SELECTION_OPTIONS_MAX);
    dispenseProductsMenuOptions.fill(DUMMY_PNUMBER);
    for (int slot_index = 0; slot_index < getSlotCount(); slot_index++)
    {
        qDebug() << "slot: " << (slot_index + 1);
        QVector<int> dispense_pnumbers = getAllDispensePNumbersFromSlot(slot_index + 1);
        for (int i = 0; i < dispense_pnumbers.size(); i++)
        {
            int position = 1 + slot_index * DISPENSE_PRODUCTS_PER_BASE_LINE_MAX + i;
            setProductToMenuOption(position, dispense_pnumbers[i]);
            qDebug() << "pnumber : " << (dispense_pnumbers[i]) << "at option" << position;
        }
    }

    // display all options:
    for (int i = 0; i < dispenseProductsMenuOptions.size(); ++i)
    {
        int product = dispenseProductsMenuOptions[i];
        qDebug() << "Option " << (i + 1) << ": " << product;
    }
}

void machine::setProductToMenuOption(int productOption, int pnumber)
{
    // options array layout containing P-number for a certain menu position. [base1option1, base1option2, base1option3, base1option4, base1option5, base1option6,
    // base2option1, base2option2, base2option3, base2option4, base2option5, base2option6,
    // base3option1, base3option2, base3option3, base3option4, base3option5, base3option6, ...
    // options start at 1
    qDebug() << "Set up pnumber: " << pnumber << " To option: " << productOption;
    if (productOption == 0)
    {

        qDebug() << "ERROR: option  numbers start from 1!!!";
    }
    if (productOption > MENU_PRODUCT_SELECTION_OPTIONS_MAX)
    {
        qDebug() << "ERROR: Maximum amount of options exceeded. Will not add as an option. ";
        return;
    }

    dispenseProductsMenuOptions[productOption - 1] = pnumber;
}

bool machine::isOptionExisting(int productOption)
{
    if (productOption == 0)
    {
        qDebug() << "ERROR:  option numbering start from 1!!!";
    }
    int pnumber = dispenseProductsMenuOptions[productOption - 1];
    return pnumber != DUMMY_PNUMBER;
}

pnumberproduct *machine::getProductFromMenuOption(int productOption)
{
    // options are selectable products by the customer. (they must be indexed. e.g. first page: option 1 to four)
    // option start counting from 1
    // slotPosition starts at 1

    if (!isOptionExisting(productOption))
    {
        qDebug() << "ASSERT ERROR: non existing number (dummy or not valid). undefined behaviour from now on" << productOption;
    }

    int pnumber = dispenseProductsMenuOptions[productOption - 1];
    qDebug() << "Get product with pnumber " << pnumber << "from product option:  " << productOption;
    return getProductByPNumber(pnumber);
}

pnumberproduct *machine::getSlotBaseProduct(int slot)
{
    int basePNumber = m_slots[slot - 1].getBasePNumber();
    return getProductByPNumber(basePNumber);
}

void machine::setSelectedProductByOption(int productOption)
{
    int pnumber = dispenseProductsMenuOptions[productOption - 1];
    m_selectedProduct = getProductByPNumber(pnumber);
}

bool machine::getIsOptionAvailable(int productOption)
{
    // available as in: is it enabled, not empty, no technical problem,... (assumes the option exists and is linked to a valid pnumber)

    // products will need an "isEnabled" and "statustext" column too.

    // check if slot for option is valid
    // check if all pnumbers for options are valid

    return true;
}

int machine::getOptionCount()
{
    if (!isAelenPillarElseSoapStand())
    {
        return getSlotCount();
    }

    // number of set options.
    int count = dispenseProductsMenuOptions.size();
    if (count > MENU_PRODUCT_SELECTION_OPTIONS_MAX)
    {
        qDebug() << "WARNING: exceeded maximum options count, will return max. ";
        return MENU_PRODUCT_SELECTION_OPTIONS_MAX;
    }
    qDebug() << "get option scoun ts : " << count;
    return count;
}

int machine::getSlotFromBasePNumber(int base_pnumber)
{
    int occurences_of_base_pnumber = 0;
    int slot_with_base_pnumber;

    for (uint8_t slot_index = 0; slot_index < getSlotCount(); slot_index++)
    {
        int base_pnumber_in_slot = m_slots[slot_index].getBasePNumber();
        if (base_pnumber == base_pnumber_in_slot)
        {
            occurences_of_base_pnumber++;
            slot_with_base_pnumber = slot_index + 1;
        }
    }

    if (occurences_of_base_pnumber == 0)
    {
        qDebug() << "Error: Searched pnumber not set as Base Pnumber in any slot. ";
        return 666;
    }
    if (occurences_of_base_pnumber > 1)
    {
        qDebug() << "Warning: Searched pnumber set as Base Pnumber in multiple slots. Will return last slot where it occured. Found in how many slots?: " << occurences_of_base_pnumber;
    }
    return slot_with_base_pnumber;
}

dispenser_slot *machine::getSlotFromOption(int productOption)
{
    qDebug() << "Product option" << productOption;
    int slot = static_cast<int>(std::round(productOption / DISPENSE_PRODUCTS_PER_BASE_LINE_MAX));
    return &m_slots[slot];
}

pnumberproduct *machine::getSelectedBottle()
{
    return m_selectedBottle;
}

void machine::setSelectedBottle(int pnumber)
{
    // pnumber is the index. Clever... until you have one million options....
    m_selectedBottle = getProductByPNumber(pnumber);
}

void machine::resetSelectedBottle()
{
    m_selectedBottle = NULL;
}

bool machine::hasSelectedBottle()
{
    if (getSelectedBottle())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool machine::hasBuyBottleOption()
{
    if (m_pNumber_bottle_1 || m_pNumber_bottle_2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

QString machine::getSelectedProductAwsProductId(){
    return getAwsProductId(getSelectedProduct()->getPNumber());
}

QString machine::getAwsProductId(int pnumber){
    
    QString productId = getMachineId() + "_" + QString::number(pnumber);
    QString suffix = getProductByPNumber(pnumber)->getAwsProductIdSuffix();

    if (!suffix.isEmpty()){
        productId += "_" + suffix;
    }
    qDebug()<< "Product Id for P-" + QString::number(pnumber) + QString(": ") + productId;
    return productId;
}

pnumberproduct *machine::getProductByPNumber(int pnumber)
{
    //qDebug() << pnumber;
    return &m_pnumberproducts[pnumber];
}

pnumberproduct *machine::getSelectedProduct()
{
    return m_selectedProduct;
}

void machine::setSelectedProduct(int pnumber)
{
    // pnumber is the index. Clever... until you have one million options....
    m_selectedProduct = getProductByPNumber(pnumber);
}

dispenser_slot *machine::getSlotByPosition(int slotPosition)
{
    // DO YOU NEED TO USE THIS?
    // this program became product centered. Only use for general stuff, like maintenance mode.

    // slotPosition starts at 1
    if (slotPosition == 0)
    {

        qDebug() << "ERROR: slot numbers start from 1!!!";
    }
    return &m_slots[slotPosition - 1];
}

void machine::setSlots(dispenser_slot *slotss)
{
    m_slots = slotss; // slots is a TYPE in QT. so we can't use it as a variable name
}

dispenser_slot *machine::getSelectedSlot()
{

    return m_selectedSlot;
}

void machine::setSelectedSlot(int slot)
{
    m_selectedSlot = &m_slots[slot - 1];
}
void machine::setSelectedSlotFromSelectedProduct()
{
    // FOR NOW this will only return a slot if the selected product has a base_product.

    // check base product from selected product
    // check slot of base product.

    int base_pnumber = m_selectedProduct->getFirstMixPNumberOrPNumberAsBasePNumber(); // if this is not a mix, it will return the main p number.

    qDebug() << "Base pnumber : " << base_pnumber;
    int slot = getSlotFromBasePNumber(base_pnumber);
    setSelectedSlot(slot);
    qDebug() << "set selected slot: " << slot;
}

bool machine::isDispenseAreaBelowElseBesideScreen()
{
    // check in database if hardware_version starts with AP or SS
    // get hardware_version from db_manager
    // if starts with SS return false, if starts with ap reeturn true

    if (m_hardware_version.startsWith("SS1"))
    {
        return false;
    }
    return true;
}

bool machine::isAelenPillarElseSoapStand()
{
    // check in database if hardware_version starts with AP or SS
    // get hardware_version from db_manager
    // if starts with SS return false, if starts with ap reeturn true

    if (m_hardware_version.startsWith("AP"))
    {
        return true;
    }
    else if (m_hardware_version.startsWith("SS"))
    {
        return false;
    }
    else
    {
        qDebug() << "ASSERT ERROR: Unknown Hardware version:  " << m_hardware_version;
    }
    return false; // default case
}

QString machine::getHardwareMajorVersion()
{
    // e.g. AP2, SS1, ...
    return m_hardware_version.left(3); //
}

bool machine::isSlotExisting(int slot)
{
    return slot <= getSlotCount();
}

int machine::getSlotCount()
{
    // check hardwarenumber
    int slot_count;
    if (m_hardware_version.startsWith("AP"))
    {
        if (m_hardware_version.startsWith("AP1"))
        {
            slot_count = 4;
        }
        else if (m_hardware_version.startsWith("AP1.1"))
        {
            slot_count = 4;
        }
        else if (m_hardware_version.startsWith("AP2"))
        {
            // get slot count dynamically from slots (number of records, check also with slot id, to make sure there are no 'gaps' (e.g. 1,2,3,4,5  instead of 1,4,5,6,7))
            slot_count = m_dispense_buttons_count % 1000;
            // qDebug() << "Slot dispensers Count for AP2:" << slot_count;
        }
        else
        {
            slot_count = 4;
        }
    }
    else if (m_hardware_version.startsWith("SS"))
    {
        if (m_hardware_version.startsWith("SS1"))
        {
            slot_count = 4;
        }
        else if (m_hardware_version.startsWith("SS2"))
        {
            slot_count = 3;
            qDebug() << " ss2   slot count 3";
        }
        else
        {
            qDebug() << "Warning, unrecognized Hardware version. Version in db: " << m_hardware_version;
            slot_count = 4;
        }
    }
    if (isSlotCountBiggerThanMaxSlotCount(slot_count))
    {
        slot_count = MAX_SLOT_COUNT;
    }
 
    return slot_count;
    // dispensers is the same as slots.

    // return m_dispense_buttons_count % 1000;
}

bool machine::isSlotCountBiggerThanMaxSlotCount(int slot_count)
{
    bool exceeded = slot_count > MAX_SLOT_COUNT;
    if (exceeded)
    {
        qDebug() << "ERROR - Slot Count:" << slot_count << " exceeded MAX_SLOT_COUNT:" << MAX_SLOT_COUNT << "threshold";
    }
    return exceeded;
}

void machine::dispenseButtonLightsAnimateState(bool animateElseOff)
{
    // if there are no button animation programs available, do nothing...
    int m_button_animation_program = m_dispense_buttons_count / 1000;
    if (m_button_animation_program > 0)
    {
        if (animateElseOff)
        {
            dfUtility->send_command_to_FSM("DispenseButtonLights|ANIMATE", true);
        }
        else
        {
            dfUtility->send_command_to_FSM("DispenseButtonLights|OFF", true);
        }
    }
}

void machine::resetUserState()
{
    setRole(UserRole::user);
    resetSessionId();  // fixme! bug! if manually exited, will not reset session id.
    initCouponState(); // everything coupon is reset when idle page is reached.
}

void machine::setCouponState(StateCoupon state)
{
    m_stateCoupon = state;
}
StateCoupon machine::getCouponState()
{
    return m_stateCoupon;
}

void machine::initCouponState()
{
    setDiscountPercentageFraction(0.0);
    setCouponCode("");
    m_max_dollar_amount_discount = "0";

    if (getCouponsEnabled())
    {
        qDebug() << "Machine: Coupons enabled. Coupon state initialized.";
        m_stateCoupon = enabled_not_set;
    }
    else
    {
        qDebug() << "Machine: Coupons disabled.";
        m_stateCoupon = disabled;
    }
}

void machine::setRebootState(StateReboot state)
{
    m_stateReboot = state;
}
StateReboot machine::getRebootState()
{
    return m_stateReboot;
}

void machine::setDiscountPercentageFraction(double percentageFraction)
{
    // ratio = percentage / 100;
    // qDebug() << "Set discount percentage as a fraction. " << QString::number(percentageFraction, 'f', 3);
    m_discount_percentage_fraction = percentageFraction;
}

void machine::resetCouponDiscount()
{
    m_discount_percentage_fraction = 0;
    m_min_threshold_vol_ml_discount = "0";
    m_max_threshold_vol_ml_discount = "0";
    m_max_dollar_amount_discount = "0";
    // setCouponCode("");
}

double machine::getDiscountPercentageFraction()
{
    return m_discount_percentage_fraction;
}

double machine::getDiscountAmount(double price)
{
    // the discount is the original price minus the discounted price
    return price - getPriceWithDiscount(price);
}

double machine::getPriceWithDiscount(double price)
{
    // will take a base price.
    // there is a maximum absolute number of discount available.
    // will return discounted price
    double discount = price * m_discount_percentage_fraction;
    // qDebug() << "Discount: " << m_max_dollar_amount_discount;
    max_discount = m_max_dollar_amount_discount.toDouble();
    double resulting_discount = (max_discount == 0.0) ? discount : std::min(max_discount, discount); // if discount > max_discount, take max_discount
    return (price - resulting_discount);
}

double machine::getPriceCorrectedForSelectedSize(int pnumber, bool maximumVolumeForCustom)
{
    double price;
    pnumberproduct *product = getProductByPNumber(pnumber);
    if (product->is_valid_size_selected())
    {
        qDebug() << "Selected size" << product->getSelectedSize();
        price = product->getBasePrice(product->getSelectedSize());
        if (maximumVolumeForCustom && (product->getSelectedSize() == SIZE_CUSTOM_INDEX))
        {
            // price is per ml for custom size, so, we multiply it by the maximum amount of dispensed volume if
            price *= product->getVolumeBySize(SIZE_CUSTOM_INDEX); // e.g. 0.01 $/ml --> max 2000ml --> 20$
        }

        price = getPriceWithDiscount(price);
    }
    else
    {
        qDebug() << "ERROR: no size set";
        price = 66.6;
    }
    return price;
}

QString machine::getCouponCode()
{
    return m_promoCode;
}

void machine::setCouponCode(QString promoCode)
{
    qDebug() << "Set Promo Code: " << promoCode;
    m_promoCode = promoCode;
}

void machine::loadElementPropertiesFile()
{
    QString json_path = getTemplatePathFromName(UI_ELEMENT_PROPERTIES_NAME);
    QFile file(json_path);
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray jsonData = file.readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
        m_propertiesObject = jsonDoc.object();

        file.close();

        // Now you have the propertiesObject containing properties for listed elements
    }

    // QList<QPushButton *> buttonList = centralWidget->findChildren<QPushButton *>();

    // foreach (QPushButton *button, buttonList) {
    //     qDebug() << "Button text:" << button->text();
    // }
}

QString machine::getTemplateFolder()
{

    // check for exact template folder.
    // if it doesn't exist, check for hardware default template folder.
    // if it doesn't exist, check for default template folder.
    QString template_name = m_template;
    if (template_name.isEmpty())
    {
        QString hardware_specific_template = QString(TEMPLATES_ROOT_PATH) + QString(TEMPLATES_DEFAULT_NAME) + "_" + getHardwareMajorVersion();
        if (!df_util::pathExists(hardware_specific_template))
        {
            template_name = QString(TEMPLATES_DEFAULT_NAME);
        }
        else
        {
            template_name = QString(TEMPLATES_DEFAULT_NAME) + "_" + getHardwareMajorVersion();
        }
    }
    QString templateFolder = QString(TEMPLATES_ROOT_PATH) + template_name + "/";
    // qDebug() << "Get template folder : " << templateFolder;
    return templateFolder;
}

QString machine::getTemplatePathFromName(QString fileName)
{
    // will try to find the file in specif template, default_hardware and default in this sequence. Return path from first found

    QString filePath = getTemplateFolder() + fileName; // this checks just for the existence of the most relevant template folder

    if (!df_util::pathExists(filePath))
    {
        // qDebug() << "File not found in template folder : " + filePath;

        // check if file exist in hardware specific default template
        // e.g.  // "/home/df-admin/production/references/templates/default_SS2/page_idle.css"
        filePath = QString(TEMPLATES_ROOT_PATH) + QString(TEMPLATES_DEFAULT_NAME) + "_" + getHardwareMajorVersion() + "/" + fileName;

        if (!df_util::pathExists(filePath))
        {
            // qDebug() << "File not found in default hardware folder: " + filePath + " Will try default.";

            // check if file exists in default template
            filePath = QString(TEMPLATES_ROOT_PATH) + QString(TEMPLATES_DEFAULT_NAME) + "/" + fileName;
            if (!df_util::pathExists(filePath))
            {
                qDebug() << "File not found in template folder and not in default hardware folder and not in default template folder (will use path anyways...): " + fileName;
            }
        }
    }
    return filePath;
}

// QString machine::getHardwareDefaultTemplatePathFromName(QString fileName)
// {

//     return TEMPLATES_ROOT_PATH + "default_" + getHardwareMajorVersion + "/" + fileName;
// }

// QString machine::getDefaultTemplatePathFromName(QString fileName)
// {
//     // last resort super default.
//     return TEMPLATES_ROOT_PATH + TEMPLATES_DEFAULT_NAME + "/" + fileName;
// }

// QString machine::getTemplatePathFromName(QString fileName)
// {
//     QString image_path = getTemplateFolder() + fileName;

//     if (!df_util::pathExists(image_path))
//     {
//         QString image_default_path = getDefaultTemplatePathFromName(fileName);
//         if (!df_util::pathExists(image_default_path))
//         {
//             qDebug() << "File not found in template folder and not in default template folder (will use path anyways...): " + image_default_path;
//         }
//         image_path = image_default_path;
//     }
//     return image_path;
// }

// QString machine::getDefaultTemplatePathFromName(QString fileName)
// {
//     QString template_root_path = TEMPLATES_ROOT_PATH;
//     return template_root_path + TEMPLATES_DEFAULT_NAME + "/" + fileName;
// }

void machine::getPrinterStatusFromDb(bool *isOnline, bool *hasPaper)
{
    qDebug() << "DB call: Check printer status. ";
    m_db->getPrinterStatus(isOnline, hasPaper);
}
void machine::writeTemperatureToDb(double temperature_1, double temperature_2)
{
    // qDebug() << "DB call: Add temperature record ";
    double defaultTemperature2 = 0.0; // Default value for temperature2
    QString defaultAlert = "";        // Default alert message (can be adjusted as needed)
    m_db->addTemperature(getMachineId(), temperature_1, temperature_2, defaultAlert);
}

void machine::checkForHighTemperatureAndDisableProducts()
{
    if (isTemperatureTooHigh_1())
    {
        if (!isTemperatureTooHigh)
        {
            isTemperatureTooHigh = true;
            temperatureTooHighStartMillis = QTime::currentTime(); // Record the time when temperature became too high
        }

        QTime currentTime = QTime::currentTime();
        int elapsedMinutes = temperatureTooHighStartMillis.msecsTo(currentTime) / 60000; // Convert milliseconds to minutes 60000=60min

        if (elapsedMinutes >= 60) // 60  Check if one hour has passed
        {
            // for (uint8_t slot_index = 0; slot_index < getSlotCount(); slot_index++)
            // {
            //     qDebug() << "Temperature too high for one hour, block all slots.";
            //     setSlotEnabled(slot_index + 1, true, "SLOT_STATE_DISABLED_COMING_SOON");
            // }
            setIsMachineEnabled(false, "SLOT_STATE_DISABLED_COMING_SOON");
        }
    }
    else
    {
        isTemperatureTooHigh = false; // Reset the flag and the time when the temperature goes back to normal
    }
}
bool machine::isTemperatureTooHigh_1()
{
    // qDebug() << "alert temperature: " << m_alert_temperature;
    // qDebug() << "current temperature: " << m_temperature;
    if (m_alert_temperature > 100.0) // if higher than 100 we consider the value bogus (sensor broken, dummy value...)
    {
        return false;
    }
    return m_temperature > m_alert_temperature;
}

void machine::fsmReceiveTemperature(double temperature_1, double temperature_2)
{
    m_temperature = temperature_1;
    m_temperature2 = temperature_2;
    // qDebug() << "Temperature received from FSM in machine: " << m_temperature;
    // qDebug() << "Temperature 2 received from FSM in machine: " << m_temperature2;

    writeTemperatureToDb(m_temperature, m_temperature2);

    if (isTemperatureTooHigh_1())
    {
        // ui->label_printer_status->setText("Temperature= " + QString::number(temperature, 'f', 2) + " is too high")
        qDebug() << "Temperature 1 too high";
    }
}

double machine::getTemperature_1()
{
    return m_temperature;
}

QString machine::getSizeUnit()
{
    return m_size_unit;
}

void machine::getTemperatureFromController()
{
    dfUtility->send_command_to_FSM("getTemperature", false);
}

bool machine::hasReceiptPrinter()
{
    return (m_has_receipt_printer == 1);
}

bool machine::getPumpRampingEnabled()
{
    return m_enable_pump_ramping == 1;
}

void machine::setPumpRampingEnabled(bool isEnabled)
{

    m_db->updateTableMachineWithInt("enable_pump_ramping", isEnabled);
}

void machine::setEmptyContainerDetectionEnabled(bool isEnabled)
{
    m_db->updateTableMachineWithInt("has_empty_detection", isEnabled);
}

bool machine::getEmptyContainerDetectionEnabled()
{
    return m_has_empty_detection == 1;
}
bool machine::getShowTransactionHistory()
{
    return m_show_transactions == 1;
}

void machine::processRolePassword(QString password_input)
{
    int compareResult_maintainer = QString::compare(password_input, m_maintenance_pwd, Qt::CaseInsensitive);
    int compareResult_admin = QString::compare(password_input, m_admin_pwd, Qt::CaseInsensitive);

    // if admin password is the same as maintainer password, make sure to set as admin!
    if (compareResult_admin == 0)
    {
        setRole(UserRole::admin);
    }
    else if (compareResult_maintainer == 0)
    {
        setRole(UserRole::maintainer);
    }
    else
    {
        setRole(UserRole::user);
        qDebug() << "Provided password not correct. Check machine tabel in database or contact soapstand.";
    }
}

QString machine::getSessionId()
{
    return m_session_id;
}

bool machine::isSessionLocked()
{
    bool sessionActive = false;
    if (getRole() != UserRole::user)
    {
        sessionActive = true;
    }
    if (getCouponState() == enabled_valid_active)
    {
        sessionActive = true;
    }
    return sessionActive;
}

void machine::resetSessionId()
{
    m_session_id = "";
}

void machine::createSessionId()
{
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    m_session_id = time;
    qDebug() << "Create Session id: " << m_session_id;
}

void machine::setRole(UserRole role)
{
    if (active_role != role)
    {
        active_role = role;
        qDebug() << "Role set as: " << getActiveRoleAsText();
    }
}

UserRole machine::getRole()
{
    return active_role;
}

QString machine::getActiveRoleAsText()
{
    switch (active_role)
    {
        // WARNING: DO NOT CHANGE TEXT, change in the templates ui_texts.csv if displaying the text needs to be different.
    case user:
        return "user";
    case maintainer:
        return "maintainer";
    case admin:
        return "admin";
    default:
        return "Unknown";
    }
}

bool machine::isAllowedAsAdmin()
{
    bool allowed = false;
    if (active_role == UserRole::admin)
    {
        allowed = true;
    }
    qDebug() << "Allowed as admin? " << allowed;
    return allowed;
}

bool machine::isAllowedAsMaintainer()
{
    bool allowed = false;
    if (active_role == UserRole::maintainer || active_role == UserRole::admin)
    {
        allowed = true;
    }
    return allowed;
}

///////////////// START MOVE TO DISPENSER CLASS
void machine::setIsMachineEnabled(bool isEnabled)
{
    m_is_enabled = isEnabled;
    m_db->updateTableMachineWithInt("is_enabled", m_is_enabled);
}

bool machine::getIsMachineEnabled()
{
    // this should have been part of the products table. But it isn't. We access this from the dispenser_slot.cpp class.
    // slotNumberValidityCheck(slot);
    // return (m_is_enabled_slots[slot - 1] == 1);
    return m_is_enabled;
}

void machine::setIsMachineEnabled(bool isEnabled, QString statusText)
{
    setIsMachineEnabled(isEnabled);

    //  m_slots[slot-1].setSlotStatusText();
    //  m_slots[slot-1].setSlotEnabled();
}

QString machine::getMachineStatusText()
{
    // slotNumberValidityCheck(slot);
    // return m_status_text_slots[slot - 1];
    return m_status_text;
}

void machine::setMachineStatusText(QString status)
{
    m_status_text = status;
    m_db->updateTableMachineWithText("status_text", m_status_text);
}

///////////////// END MOVE TO DISPENSER CLASS

QString machine::getPumpId(int slot)
{
    slotNumberValidityCheck(slot);
    return m_pump_id_slots[slot - 1];
}

void machine::registerUserInteraction(QWidget *page)
{
    QString page_name = page->objectName();
    qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< User entered: " + page_name + " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
    QString event = "Entered Page";
    m_db->addUserInteraction(getSessionId(), getActiveRoleAsText(), page_name, event);
}

bool machine::slotNumberValidityCheck(int slot)
{
    bool valid = true;
    if (slot < 1)
    {
        valid = false;
    }
    if (slot > getSlotCount())
    {
        valid = false;
    }
    if (!valid)
    {

        qDebug() << "Invalid slot! slots start from 1 and go up to " << getSlotCount() << " e.g. 1,2,3,4. Slot provided: " << slot;
    }
    return valid;
}

QString machine::getHelpPageHtmlText()
{
    return m_help_text_html;
}

void machine::resetTransactionLogging()
{
    transactionLogging = "";
}

void machine::addToTransactionLogging(QString text)
{
    transactionLogging += text;
    qDebug() << "Transaction Logging: " << transactionLogging;
}

QString machine::getTransactionLogging()
{
    return transactionLogging;
}

bool machine::isMachineDBLoaded()
{
    return m_machine_database_table_loaded_successfully;
}
bool machine::isSlotsLoaded()
{
    return m_slots_loaded_successfully;
}
bool machine::isProductsLoaded()
{
    return m_products_loaded_successfully;
}

bool machine::loadMachineParameterFromDb()
{
    qDebug() << "DB call: Load all machine parameters";

    m_machine_database_table_loaded_successfully = m_db->getAllMachineProperties(
        &m_machine_id,
        &m_client_id,
        &m_template,
        &m_location,
        &m_controller_type,
        &m_controller_id,
        &m_screen_type,
        &m_screen_id,
        &m_has_receipt_printer,
        &m_receipt_printer_is_online,
        &m_receipt_printer_has_paper,
        &m_has_tap_payment,
        &m_hardware_version,
        &m_software_version,
        &m_aws_port,
        &m_coupons_enabled,
        &m_has_empty_detection,
        &m_enable_pump_ramping,
        &m_enable_pump_reversal,
        &m_dispense_buttons_count,
        &m_maintenance_pwd,
        &m_show_transactions,
        &m_help_text_html,
        &m_idle_page_type,
        &m_admin_pwd,
        m_pump_id_slots,
        m_is_enabled_slots,
        m_status_text_slots,
        &m_alert_temperature,
        &m_software_version_controller,
        &m_is_enabled,
        &m_status_text,
        &m_paymentOptions,
        &m_size_unit,
        &m_screen_sleep_time24h,
        &m_screen_wakeup_time24h,
        &m_pNumber_bottle_1,
        &m_pNumber_bottle_2,
        &m_portal_base_url,
        &m_enable_offline_payment,
        &m_page_init_timeout);

    qDebug() << "Machine ID as loaded from db: " << getMachineId();
    qDebug() << "Template folder: " << getTemplateFolder();
    return m_machine_database_table_loaded_successfully;
}

QString machine::getIdlePageType()
{
    return m_idle_page_type;
}

bool machine::getCouponsEnabled()
{

    return m_coupons_enabled==1;
}

QString machine::getPaymentOptions()
{
    // string from the db, can be multiple options e.g. tap_canada_qr
    return m_paymentOptions;
}

void machine::setPaymentOptionsInDb(QString paymentMethod)
{
    qDebug() << "Open db: set payment method";
    m_db->updateTableMachineWithText("payment", paymentMethod);
}

ActivePaymentMethod machine::getSelectedPaymentMethod()
{
    // selected payment method for the current session. 
    return m_selectedPaymentMethod;
}

void machine::setSelectedPaymentMethod(ActivePaymentMethod paymentMethod)
{
    m_selectedPaymentMethod = paymentMethod;
}

std::vector<ActivePaymentMethod> machine::getAllowedPaymentMethods()
{
    return allowedPaymentMethods;
}

void machine::setAllowedPaymentMethods(ActivePaymentMethod paymentMethod)
{
    allowedPaymentMethods.push_back(paymentMethod);
}

QString machine::getMachineId()
{
    return m_machine_id;
}

QString machine::getMachineLocation()
{
    return m_location;
}

QString machine::getClientId()
{
    QString id = m_client_id;
    if (m_client_id.at(0) != 'C')
    {
        qDebug() << "WARNING: invalid client ID. Should be with a format like C-1, C-374, ... . Provided id: " << id;
    }
    return m_client_id;
}

void machine::setCouponConditions(QString couponConditions)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(couponConditions.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();
    m_min_threshold_vol_ml_discount = jsonObj["min_threshold_vol_ml"].toString();
    m_max_threshold_vol_ml_discount = jsonObj["max_threshold_vol_ml"].toString();
    m_max_dollar_amount_discount = jsonObj["max_dollar_amount_discount"].toString();
}

std::map<QString, QString> machine::getCouponConditions()
{
    std::map<QString, QString> myMap;
    myMap["m_min_threshold_vol_ml_discount"] = m_min_threshold_vol_ml_discount;
    myMap["m_max_threshold_vol_ml_discount"] = m_max_threshold_vol_ml_discount;
    myMap["m_max_dollar_amount_discount"] = m_max_dollar_amount_discount;
    return myMap;
}

void machine::addClientLogoToLabel(QLabel *label)
{
    QString id = getClientId();
    QString logo_path = QString(CLIENT_LOGO_PATH).arg(id);
    addPictureToLabel(label, logo_path);
}

void machine::addPictureToButton(QPushButton *button, QString picturePath)
{
    if (df_util::pathExists(picturePath))
    {
        QPixmap im(picturePath);
        QIcon qi(im);
        button->setIcon(qi);
        button->setIconSize(QSize(271, 391));
    }
}

void machine::addPictureToLabel(QLabel *label, QString picturePath)
{
    if (df_util::pathExists(picturePath))
    {
        QPixmap picture(picturePath);

        int w = label->width();
        int h = label->height();
        // // // set a scaled pixmap to a w x h window keeping its aspect ratio
        label->setPixmap(picture.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else
    {
        qDebug() << "Can't add picture to label: " << label->objectName() << " " << picturePath;
    }
}

void machine::addPictureToLabelCircle(QLabel *label, QString picturePath)
{
    if (df_util::pathExists(picturePath))
    {
        QPixmap picture(picturePath);
        QPixmap mask(picture.size());
        mask.fill(Qt::transparent);

        // Draw smooth ellipse on mask
        QPainter painter(&mask);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(QBrush(Qt::white));
        painter.drawEllipse(0, 0, picture.width(), picture.height());

        // Apply mask to picture
        picture.setMask(mask.mask());

        // Set a scaled pixmap to a w x h window keeping its aspect ratio
        int w = label->width();
        int h = label->height();
        label->setPixmap(picture.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else
    {
        qDebug() << "Can't add picture to label: " << label->objectName() << " " << picturePath;
    }
}

void machine::addCssClassToObject(QWidget *element, QString classname, QString css_file_name)
{
    QString styleSheet = getCSS(css_file_name);
    element->setProperty("class", classname);
    element->setStyleSheet(styleSheet);
}

QString machine::getCSS(QString cssName)
{
    QString cssFilePath = getTemplatePathFromName(cssName);

    QFile cssFile(cssFilePath);
    QString styleSheet = "";
    if (cssFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        styleSheet = QString::fromUtf8(cssFile.readAll());
    }
    else
    {
        qDebug() << "CSS file could not be opened." << cssFilePath;
    }
    return styleSheet;
}

void machine::pageTransition(QWidget *pageToHide, QWidget *pageToShow)
{
    // page transition effects are not part of QT but of the operating system! // search for ubuntu settings program to set transition animations to "off"
    pageToShow->showFullScreen();
    pageToHide->hide();
}

void machine::setBackgroundPictureFromTemplateToPage(QWidget *p_widget, QString imageName)
{
    // on Page: if called from setPage: will not scale
    // on Page: if called from showEvent: will scale to screen

    QString image_path = imageName;
    image_path = getTemplatePathFromName(imageName);
    // qDebug()<< "background image path: " << image_path;
    setBackgroundPictureToQWidget(p_widget, image_path);
}

void machine::setBackgroundPictureToQWidget(QWidget *p_widget, QString image_path)
{
    QPixmap background(image_path);

    if (background.isNull()) {
        qDebug() << "Failed to load image. Check if the file exists and the path is correct.";
    } else {
        QPalette palette;
        palette.setBrush(QPalette::Background, background);
        p_widget->setPalette(palette);
        p_widget->setAutoFillBackground(true); // Ensure autoFillBackground is set to true
        p_widget->repaint();
        p_widget->update();
    }

}

void machine::setTemplateTextWithIdentifierToObject(QWidget *p_element, QString identifier)
{
    QString text = getTemplateTextByElementNameAndPageAndIdentifier(p_element, identifier);
    setTextToObject(p_element, text);
}

QString machine::getTemplateTextByElementNameAndPageAndIdentifier(QWidget *p_element, QString identifier)
{
    // QString element_page_and_name = getTemplateTextByElementNameAndPage(p_element);
    QString element_page_and_name = getCombinedElementPageAndName(p_element);
    QString searchString = element_page_and_name + "->" + identifier;
    return getTemplateText(searchString);
}

void machine::setTemplateTextToObject(QWidget *p_element)
{
    QString searchString = getTemplateTextByElementNameAndPage(p_element);
    setTextToObject(p_element, searchString);
}

QString machine::getCombinedElementPageAndName(QWidget *p_element)
{
    QString elementName = p_element->objectName();
    QWidget *parentWidget = p_element->parentWidget();

    if (!parentWidget)
    {
        qDebug() << "No parent for the provided widget!! " << elementName;
    }

    QString pageName = parentWidget->objectName();

    return pageName + "->" + elementName;
}

QString machine::getTemplateTextByElementNameAndPage(QWidget *p_element)
{
    QString pageName_elementName_combination = getCombinedElementPageAndName(p_element);
    return getTemplateText(pageName_elementName_combination);
}

void machine::setTextToObject(QWidget *p_element, QString text)
{
    if (QLabel *label = qobject_cast<QLabel *>(p_element))
    {
        label->setText(text);
    }
    else if (QPushButton *button = qobject_cast<QPushButton *>(p_element))
    {
        button->setText(text);
    }
    else if (QMessageBox *msgBox = qobject_cast<QMessageBox *>(p_element))
    {
        msgBox->setText(text);
    }
    else
    {
        // Handle other types of elements if needed
    }
}

// get a text that is not linked to an elements on a specific page by its identifier
QString machine::getTemplateTextByPage(QWidget *page, QString identifier)
{
    QString pageName = page->objectName();
    QString searchString = pageName + "->" + identifier;
    return getTemplateText(searchString);
}

QString machine::getTemplateText(QString textName_to_find)
{
    std::string key = textName_to_find.toStdString();
    auto it = textNameToTextMap_template.find(QString::fromStdString(key));
    QString retval;
    if (it != textNameToTextMap_template.end())
    {
        // std::cout << "Word found! Sentence: " << it->second ;
        retval = it->second;
    }
    else
    {
        it = textNameToTextMap_default_hardware.find(QString::fromStdString(key));
        if (it != textNameToTextMap_default_hardware.end())
        {
            retval = it->second;
        }
        else
        {
            it = textNameToTextMap_default.find(QString::fromStdString(key));
            if (it != textNameToTextMap_default.end())
            {
                retval = it->second;
            }
            else
            {
                qDebug() << "No template text value found for: " + textName_to_find;
                retval = textName_to_find;
            }
        }
    }

    // \n values in the csv file get automatically escaped. We need to deescape them.
    retval.replace("\\n", "\n");

    return retval;
}

void machine::applyDynamicPropertiesFromTemplateToWidgetChildren(QWidget *widget)
{
    // Template engine can set properties of a widget (like position, visibility,...) from a text file

    // in reality, send a page widget as argument. All the childeren will be checked. (i.e. buttons, labels,...)
    QList<QObject *> allChildren = widget->findChildren<QObject *>();
    foreach (QObject *child, allChildren)
    {
        QWidget *widget = qobject_cast<QWidget *>(child);
        if (widget)
        {
            // not all child element are widgets.

            QString combinedName = getCombinedElementPageAndName(widget);
            // qDebug() << combinedName;
            applyPropertiesToQWidget(widget);
        }
    }
}

void machine::applyPropertiesToQWidget(QWidget *widget)
{

    // example of line in text file with properties:
    // machine->label_client_logo,{"x":570, "y":1580, "width":351, "height":211,"isVisibleAtLoad":true}

    QString combinedName = getCombinedElementPageAndName(widget);

    auto selectedElement = elementDynamicPropertiesMap_template.find(combinedName); // find element in associative array
    QString propertiesJsonString;
    bool valid = true;

    if (selectedElement != elementDynamicPropertiesMap_template.end())
    {
        // if found in template dynamic properties
        qDebug() << "element " << combinedName << "found in template. json string: " << selectedElement->second;
        propertiesJsonString = selectedElement->second; // second implies the value of the key:value pair in the associative array
    }
    else
    {
        // if found in default hardware template dynamic properties
        selectedElement = elementDynamicPropertiesMap_default_hardware.find(combinedName);
        if (selectedElement != elementDynamicPropertiesMap_default_hardware.end())
        {
            qDebug() << "element " << combinedName << "found in default hardware template. json string: " << selectedElement->second;
            propertiesJsonString = selectedElement->second;
        }
        else
        {
            // if found in default template dynamic properties
            selectedElement = elementDynamicPropertiesMap_default.find(combinedName);
            if (selectedElement != elementDynamicPropertiesMap_default.end())
            {
                qDebug() << "element " << combinedName << "found in default template. json string: " << selectedElement->second;
                propertiesJsonString = selectedElement->second;
            }
            else
            {
                // qDebug() << "No template text value found for: " + combinedName;
                valid = false;
            }
        }
    }

    if (!valid)
    {
        return;
    }

    QJsonObject jsonObject = df_util::parseJsonString(propertiesJsonString);

    int x = widget->x();
    int y = widget->y();
    int width = widget->width();
    int height = widget->height();

    if (jsonObject.contains("x"))
    {
        QJsonValue xValue = jsonObject.value("x");
        x = xValue.toInt();
        // qDebug() << "x found  " << x ;
    }
    if (jsonObject.contains("y"))
    {
        QJsonValue val = jsonObject.value("y");
        y = val.toInt();
        // qDebug() << "y found  " << y ;
    }
    if (jsonObject.contains("width"))
    {
        QJsonValue val = jsonObject.value("width");
        width = val.toInt();
        // qDebug() << "width found  " << width ;
    }
    if (jsonObject.contains("height"))
    {
        QJsonValue val = jsonObject.value("height");
        height = val.toInt();
        // qDebug() << "height found  " << height ;
    }
    widget->setGeometry(x, y, width, height);

    if (jsonObject.contains("isVisibleAtLoad"))
    {
        QJsonValue val = jsonObject.value("isVisibleAtLoad");
        bool isVisible = val.toBool();
        // qDebug() << "visibility found  " << isVisible ;

        widget->setVisible(isVisible);
    }
}

void machine::loadElementDynamicPropertiesFromTemplate()
{
    qDebug() << "Load dynamic properties from template file";
    QString path = QString(TEMPLATES_ROOT_PATH) + m_template + "/" + QString(UI_ELEMENT_PROPERTIES_NAME);
    loadTextsFromCsv(path, &elementDynamicPropertiesMap_template);
}
void machine::loadElementDynamicPropertiesFromDefaultHardwareTemplate()
{
    qDebug() << "Load dynamic properties from default hardware template file";
    QString path = QString(TEMPLATES_ROOT_PATH) + QString(TEMPLATES_DEFAULT_NAME) + "_" + getHardwareMajorVersion() + "/" + QString(UI_ELEMENT_PROPERTIES_NAME);
    loadTextsFromCsv(path, &elementDynamicPropertiesMap_default_hardware);
}
void machine::loadElementDynamicPropertiesFromDefaultTemplate()
{
    qDebug() << "Load dynamic properties from default template file";
    QString path = QString(TEMPLATES_ROOT_PATH) + QString(TEMPLATES_DEFAULT_NAME) + "/" + QString(UI_ELEMENT_PROPERTIES_NAME);
    loadTextsFromCsv(path, &elementDynamicPropertiesMap_default);
    // Print the word-sentence mapping
    // for (const auto &pair : elementDynamicPropertiesMap_default)
    // {
    //     qDebug() << pair.first << ": " << pair.second;
    // }
}

void machine::loadTextsFromTemplateCsv()
{
    qDebug() << "Load dynamic texts from template csv";
    QString path = QString(TEMPLATES_ROOT_PATH) + m_template + "/" + QString(UI_TEXTS_CSV_NAME);
    loadTextsFromCsv(path, &textNameToTextMap_template);
}

void machine::loadTextsFromDefaultHardwareCsv()
{
    qDebug() << "Load dynamic texts from default hardware csv";
    QString path = QString(TEMPLATES_ROOT_PATH) + QString(TEMPLATES_DEFAULT_NAME) + "_" + getHardwareMajorVersion() + "/" + QString(UI_TEXTS_CSV_NAME);
    loadTextsFromCsv(path, &textNameToTextMap_default_hardware);
}

void machine::loadTextsFromDefaultCsv()
{
    qDebug() << "Load dynamic texts from default csv";
    // QString csv_default_hardware_template_path = TEMPLATES_ROOT_PATH + TEMPLATES_DEFAULT_NAME + "_" + getHardwareMajorVersion + "/" + UI_TEXTS_CSV_NAME;
    QString path = QString(TEMPLATES_ROOT_PATH) + QString(TEMPLATES_DEFAULT_NAME) + "/" + QString(UI_TEXTS_CSV_NAME);
    loadTextsFromCsv(path, &textNameToTextMap_default);
}

void machine::loadTextsFromCsv(QString csv_path, std::map<QString, QString> *dictionary)
{
    std::ifstream file(csv_path.toStdString());
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            if (!line.empty() && line[0] != '#') // Skip empty lines and lines starting with '#'
            {
                // qDebug() << QString::fromStdString(line);

                std::size_t delimiter_pos = line.find(','); // left of , is the element name, right is its text
                if (delimiter_pos != std::string::npos)
                {
                    std::string word = line.substr(0, delimiter_pos);
                    std::string sentence = line.substr(delimiter_pos + 1);
                    QString qword = QString::fromStdString(word);
                    QString qsentence = QString::fromStdString(sentence);
                    (*dictionary)[qword] = qsentence;
                }
            }
        }
        file.close();

        // Print the word-sentence mapping
        // for (const auto &pair : *dictionary)
        // {
        //     qDebug() << pair.first << ": " << pair.second;
        // }
    }
    else
    {
        qDebug() << "Texts file path could not be opened: " + csv_path;
    }
}

QStringList machine::getChildNames(QObject *parent)
{
    QStringList childNames;

    if (!parent)
    {
        qDebug() << "Invalid parent object.";
        return childNames;
    }

    QList<QObject *> allChildren = parent->findChildren<QObject *>();

    foreach (QObject *child, allChildren)
    {
        childNames.append(child->objectName());
    }

    return childNames;
}

bool machine::hasMixing()
{
    if (m_hardware_version == "AP2" || m_hardware_version == "AP3")
    {
        return true;
    }
    else
    {
        return false;
    }
}

QString machine::getPortalBaseUrl(){
    return m_portal_base_url;
}

bool machine::isEnabledOfflinePayment(){
    return m_enable_offline_payment;
}

void machine::setFreeSampleEndURL(QString ending_url){
    m_freesample_end_url = ending_url;
}

QString machine::getFreeSampleEndURL(){
    return m_freesample_end_url;
}

size_t WriteCallback2(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

int machine::getPageInitTimeout(){
    return m_page_init_timeout;
}

std::tuple<CURLcode, std::string, long> machine::sendRequestToPortal(QString api_url, QString request_type, QString curl_params, QString page_name){

    api_url = m_portal_base_url + api_url;
    
    curl_param_array = curl_params.toLocal8Bit();
    curl_data = curl_param_array.data();

    curl = curl_easy_init();
    if (!curl)
    {
        QString failedTransactionParams = "PAGE: " + page_name + "; API_URL= " + api_url + " ; CURL_PARAMS= " + curl_params + "\n";
        qDebug() << "CURL failed to init. parameters:" + failedTransactionParams;
        // Write transaction to file
        dfUtility->write_to_file(TRANSACTION_DISPENSE_END_OFFLINE_PATH, failedTransactionParams);
        return {res, "Curl failed", 404};
    }
    curl_easy_setopt(curl, CURLOPT_URL, api_url.toUtf8().constData());
    if(request_type=="POST"){
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_param_array.data());
    }
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback2);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, SOAPSTANDPORTAL_CONNECTION_TIMEOUT_MILLISECONDS);
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    
    std::tuple<CURLcode, std::string, long> returnObject = {res,readBuffer, http_code};

    curl_easy_cleanup(curl);
    readBuffer = "";
    return returnObject;
}