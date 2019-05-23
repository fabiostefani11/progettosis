CREATE TABLE STUDENTE ( 
	NOME 	VARCHAR(15) NOT NULL,
	NUMEROSTUD 	INT(15) NOT NULL, 	
	ANNOCORSO 		INT(2) NOT NULL, 	
	CORSOLAUREA VARCHAR(30) NOT NULL,
	
	PRIMARY KEY (NUMEROSTUD)					-- Definisco la chiave primaria (PK)
	);

CREATE TABLE INSEGNAMENTO ( 
	NOMEINSEGNAMENTO 			VARCHAR(30) NOT NULL,
	CODICEINSEGNAMENTO 		VARCHAR(15) NOT NULL,
	ORE 		INT(10) NOT NULL,
	DIPARTIMENTO 	VARCHAR(15) NOT NULL,

	PRIMARY KEY (CODICEINSEGNAMENTO)				
); 

CREATE TABLE PROPEDEUTICITÀ ( 
	CODICEINSEGNAMENTO 		VARCHAR(30) NOT NULL,
	CODICEPROP 			INT(10) NOT NULL,
	
	PRIMARY KEY (CODICEPROP),					-- La PK è composta da 2 campi
	FOREIGN KEY (CODICEINSEGNAMENTO) REFERENCES INSEGNAMENTO(CODICEINSEGNAMENTO) 
);

CREATE TABLE MODULO ( 
	IDMODULO		VARCHAR(15) NOT NULL,
	CODICEINSEGNAMENTO 	VARCHAR(30) NOT NULL,
	SEMESTRE 		INT(2) NOT NULL,
	ANNO 			INT(5) NOT NULL,
	DOCENTE 		VARCHAR(20) NOT NULL,
    
	PRIMARY KEY (IDMODULO),
	FOREIGN KEY (CODICEINSEGNAMENTO) REFERENCES INSEGNAMENTO(CODICEINSEGNAMENTO) 
    );
    
CREATE TABLE VOTAZIONE (
    NUMEROSTUD INT(15) NOT NULL,
    IDMODULO VARCHAR(15) NOT NULL,
    VOTO INT(5) NOT NULL,
    FOREIGN KEY(NUMEROSTUD) REFERENCES STUDENTE(NUMEROSTUD),
    FOREIGN KEY(IDMODULO) REFERENCES MODULO(IDMODULO)
    );
