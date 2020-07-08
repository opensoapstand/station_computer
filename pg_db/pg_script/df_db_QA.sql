-- TEST inventory restriction
-- Machine cannot insert
UPDATE df_transaction.inventory
SET in_stock = TRUE
WHERE inventory_id = 0;

-- DO NOT USE ASSERT!  only for debugging
-- RAISE can be used...will look into unit testing options later...

do $$ begin INSERT INTO df_transaction.inventory
VALUES (99, NULL, NULL, current_timestamp, FALSE);

end;
$$ LANGUAGE plpgsql;