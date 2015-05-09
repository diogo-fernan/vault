PRAGMA foreign_keys = ON;

BEGIN TRANSACTION;

CREATE TABLE User (
	id 			INT PRIMARY KEY NOT NULL,
	name 		VARCHAR(20) NOT NULL,
	password 	CHAR(32) NOT NULL
);

CREATE TABLE Information (
	id 			INT PRIMARY KEY NOT NULL,
	name 		VARCHAR(20) NOT NULL,
	secret 		VARCHAR(100) NOT NULL,
	designation VARCHAR(50) NOT NULL,
	idUser 		INT NOT NULL,
	FOREIGN KEY(idUser)
		REFERENCES User(id)
		ON DELETE CASCADE
);

CREATE TABLE Log (
	id 			INTEGER PRIMARY KEY AUTOINCREMENT,
	name 		VARCHAR(20) NOT NULL,
	timedate 	DATETIME NOT NULL,
	operation 	VARCHAR(20) NOT NULL
); 

CREATE TRIGGER TriggerUserInsert
AFTER INSERT ON User
BEGIN
	INSERT INTO Log (name, timedate, operation)
	VALUES (
			"User",
			datetime('now'),
			"INSERT"
		);
END;
 
CREATE TRIGGER TriggerUserDelete
AFTER DELETE ON User
BEGIN
	INSERT INTO Log (name, timedate, operation)
	VALUES (
		"User",
		datetime('now'),
		"DELETE"
	);
END;
 
CREATE TRIGGER TriggerUserUpdate
AFTER UPDATE ON User
BEGIN
	INSERT INTO Log (name, timedate, operation)
	VALUES (
		"User",
		datetime('now'),
		"UPDATE"
	);
END;
 
CREATE TRIGGER TriggerInformationInsert
AFTER INSERT ON Information
BEGIN
	INSERT INTO Log (name, timedate, operation)
	VALUES (
		"Information",
		datetime('now'),
		"INSERT"
	);
END;
 
CREATE TRIGGER TriggerInformationDelete
AFTER DELETE ON Information
BEGIN
	INSERT INTO Log (name, timedate, operation)
	VALUES (
		"Information",
		datetime('now'),
		"DELETE"
	);
END;
 
CREATE TRIGGER TriggerInformationUpdate
AFTER UPDATE ON Information
BEGIN
	INSERT INTO Log (name, timedate, operation)
	VALUES (
		"Information",
		datetime('now'),
		"UPDATE"
	);
END;

COMMIT;