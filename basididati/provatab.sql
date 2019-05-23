
CREATE TABLE IMPIEGATO ( 
	NOME_BATT 	VARCHAR(15) NOT NULL,
	INIZ_INT 	CHAR,
	COGNOME 	VARCHAR(15) NOT NULL, 	-- NOT NULL significa che DEVE avere un valore
	SSN 		CHAR(16) NOT NULL, 	-- CHAR 13 caratteri fissi (vengono riempiti con degli spazi)
	DATA_N 		DATE, 			-- Data, YYYY-MM-DD
	INDIRIZZO 	VARCHAR(30), 		-- Stringa di lunghezza variabile
	SESSO 		CHAR, 			-- Singolo carattere
	STIPENDIO 	DECIMAL(10,2), 		-- Cifra decimale con 10 cifre significative e 2 decimali
	SUPER_SSN 	CHAR(16),
	N_D 		INT NOT NULL,
	
	PRIMARY KEY (SSN), 					-- Definisco la chiave primaria (PK)
	FOREIGN KEY (SUPER_SSN) REFERENCES IMPIEGATO(SSN) 	-- Definisco una chiave esterna (FK)
);

CREATE TABLE DIPARTIMENTO ( 
	NOME_D 			VARCHAR(15) NOT NULL,
	NUMERO_D 		INT NOT NULL,
	SSN_DIR 		CHAR(16) NOT NULL,
	DATA_INIZIO_DIR 	DATE,

	PRIMARY KEY (NUMERO_D),
	UNIQUE (NOME_D), 				-- Valore unico, non possono esserci 2 dip. con lo stesso nome
	FOREIGN KEY (SSN_DIR) REFERENCES IMPIEGATO(SSN) -- Definisco una chiave esterna (FK)
); 

ALTER TABLE IMPIEGATO 
	ADD FOREIGN KEY (N_D) REFERENCES DIPARTIMENTO(NUMERO_D);

CREATE TABLE SEDI_DIP ( 
	NUMERO_D 		INT NOT NULL,
	SEDE_D 			VARCHAR(15) NOT NULL,
	
	PRIMARY KEY (NUMERO_D, SEDE_D),					-- La PK è composta da 2 campi
	FOREIGN KEY (NUMERO_D) REFERENCES DIPARTIMENTO(NUMERO_D) 
);

CREATE TABLE PROGETTO ( 
	NOME_P 		VARCHAR(15) NOT NULL,
	NUMERO_P 	INT NOT NULL,
	SEDE_P 		VARCHAR(15),
	NUM_D 		INT NOT NULL,

	PRIMARY KEY (NUMERO_P),
	UNIQUE (NOME_P), 
	FOREIGN KEY (NUM_D) REFERENCES DIPARTIMENTO(NUMERO_D) 
);

CREATE TABLE LAVORA_SU ( 
	SSN_I 	CHAR(16) NOT NULL,
	N_P 	INT NOT NULL,
	ORE 	DECIMAL(3,1) NOT NULL,
	
	PRIMARY KEY (SSN_I, N_P),
	FOREIGN KEY (SSN_I) REFERENCES IMPIEGATO(SSN), 
	FOREIGN KEY (N_P) REFERENCES PROGETTO(NUMERO_P) 
); 

CREATE TABLE PERSONA_A_CARICO ( 
	SSN_I 					CHAR(16) NOT NULL,
	NOME_PERSONA_A_CARICO 			VARCHAR(15) NOT NULL,
	SESSO 					CHAR,
	DATA_N					DATE,
	PARENTELA 				VARCHAR(8),
	
	PRIMARY KEY (SSN_I, PARENTELA),
	FOREIGN KEY (SSN_I) REFERENCES IMPIEGATO(SSN) 
);