SET foreign_key_checks = 0;

INSERT INTO SEDI_DIP VALUES (1, 'Houston');
INSERT INTO SEDI_DIP VALUES (4, 'Stafford');
INSERT INTO SEDI_DIP VALUES (5, 'Bellaire');
INSERT INTO SEDI_DIP VALUES (5, 'Sugarland');
INSERT INTO SEDI_DIP VALUES (5, 'Austin');

INSERT INTO PROGETTO VALUES ('ProductX', 1, 'Bellaire', 5);
INSERT INTO PROGETTO VALUES ('ProductY', 2, 'Sugarland', 5);
INSERT INTO PROGETTO VALUES ('ProductZ', 3, 'Houston', 5);
INSERT INTO PROGETTO VALUES ('Computerization', 10, 'Stafford', 4);
INSERT INTO PROGETTO VALUES ('Reorganization', 20, 'Houston', 1);
INSERT INTO PROGETTO VALUES ('Newbenefits', 30, 'Stafford', 4);

INSERT INTO LAVORA_SU VALUES ('CTTGDL55L23C228R', 1, 32.0);
INSERT INTO LAVORA_SU VALUES ('CTTGDL55L23C228R', 2, 8.0);
INSERT INTO LAVORA_SU VALUES ('GNSFDN36S52C312L', 1, 20.0);
INSERT INTO LAVORA_SU VALUES ('GNSFDN36S52C312L', 2, 20.0);
INSERT INTO LAVORA_SU VALUES ('VNTGPA82R71E612D', 1, 10.0);
INSERT INTO LAVORA_SU VALUES ('VNTGPA82R71E612D', 2, 10.0);
INSERT INTO LAVORA_SU VALUES ('VNTGPA82R71E612D', 3, 5.0);
INSERT INTO LAVORA_SU VALUES ('VNTGPA82R71E612D', 10, 10.0);
INSERT INTO LAVORA_SU VALUES ('VNTGPA82R71E612D', 20, 10.0);
INSERT INTO LAVORA_SU VALUES ('VNTGPA82R71E612D', 30, 5.0);
INSERT INTO LAVORA_SU VALUES ('SCRFLC38E66A767O', 30, 30.0);
INSERT INTO LAVORA_SU VALUES ('SCRFLC38E66A767O', 10, 10.0);
INSERT INTO LAVORA_SU VALUES ('CVTRMS55L02C219T', 10, 35.0);
INSERT INTO LAVORA_SU VALUES ('CVTRMS55L02C219T', 30, 5.0);
INSERT INTO LAVORA_SU VALUES ('CMPBLN44P41F010O', 30, 20.0);
INSERT INTO LAVORA_SU VALUES ('CMPBLN44P41F010O', 20, 15.0);
INSERT INTO LAVORA_SU VALUES ('BZZCMR32C22B901Y', 20, 10.0);

SET foreign_key_checks = 1;