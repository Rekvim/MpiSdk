-- DROP TABLE (сначала удаляем зависимости)
DROP TABLE IF EXISTS valve_components;
DROP TABLE IF EXISTS cv_values;
DROP TABLE IF EXISTS valve_dn_sizes;
DROP TABLE IF EXISTS valve_models;
DROP TABLE IF EXISTS valve_series;
DROP TABLE IF EXISTS saddle_materials;
DROP TABLE IF EXISTS body_materials;
DROP TABLE IF EXISTS manufacturers;

-- CREATE TABLE
CREATE TABLE manufacturers (
    id   INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL
);

CREATE TABLE valve_series (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    manufacturer_id INTEGER,
    name            TEXT NOT NULL,
    FOREIGN KEY (manufacturer_id) REFERENCES manufacturers(id)
);

CREATE TABLE valve_models (
    id        INTEGER PRIMARY KEY AUTOINCREMENT,
    series_id INTEGER,
    name      TEXT NOT NULL,
    FOREIGN KEY (series_id) REFERENCES valve_series(id)
);

CREATE TABLE valve_dn_sizes (
    id        INTEGER PRIMARY KEY AUTOINCREMENT,
    series_id INTEGER,
    dn_size   INTEGER NOT NULL,
    FOREIGN KEY (series_id) REFERENCES valve_series(id)
);

CREATE TABLE cv_values (
    id                INTEGER PRIMARY KEY AUTOINCREMENT,
    valve_dn_size_id  INTEGER,
    cv                REAL NOT NULL,
    FOREIGN KEY (valve_dn_size_id) REFERENCES valve_dn_sizes(id)
);

CREATE TABLE saddle_materials (
    id   INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL
);

CREATE TABLE body_materials (
    id   INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL
);

CREATE TABLE valve_components (
    id                 INTEGER PRIMARY KEY AUTOINCREMENT,
    valve_dn_size_id   INTEGER,
    cv_value_id        INTEGER,
    saddle_material_id INTEGER,
    plunger            TEXT,
    saddle             TEXT,
    bushing            TEXT,
    oRingSealingRing   TEXT,
    stuffingBoxSeal    TEXT,
    driveDiaphragm     TEXT,
    setOfCovers        TEXT,
    shaft              TEXT,
    saddleLock         TEXT,
    FOREIGN KEY (valve_dn_size_id)   REFERENCES valve_dn_sizes(id),
    FOREIGN KEY (cv_value_id)        REFERENCES cv_values(id),
    FOREIGN KEY (saddle_material_id) REFERENCES saddle_materials(id)
);

-- INSERT TABLE
INSERT INTO manufacturers (name) VALUES
  ('Masoneilan'),
  ('Samson'),
  ('Fisher'),
  ('Flowserve'),
  ('ЛГ-Автоматика'),
  ('РУСТ-95'),
  ('НТ Вэлв');

INSERT INTO valve_series (manufacturer_id, name) VALUES
  (1, '41000'),
  (1, '35002'),
  (1, '28000'),
  (1, '21000');

INSERT INTO valve_models (series_id, name) VALUES
  (2, '35-35112'),
  (2, '35-35122'),
  (2, '35-35212'),
  (2, '35-35222'),
  (2, '35-35312'),
  (2, '35-35322'),
  (2, '35-35412'),
  (2, '35-35422'),
  (2, '35-35512'),
  (2, '35-35522'),
  (2, '35-35612'),
  (2, '35-35622'),
  (2, '35-35712'),
  (2, '35-35722'),
  (2, '35-35812'),
  (2, '35-35822');

INSERT INTO saddle_materials (name) VALUES
  ('316'),
  ('316 стел'),
  ('316 PTFE');

INSERT INTO body_materials (name) VALUES
  ('Нерж сталь (CF3M)'),
  ('Угл. сталь (WCC)');

INSERT INTO valve_dn_sizes (series_id, dn_size) VALUES
  (2, 25),
  (2, 40),
  (2, 50),
  (2, 80),
  (2, 100),
  (2, 150),
  (2, 200),
  (2, 250);

INSERT INTO cv_values (valve_dn_size_id, cv) VALUES
  (1, 15),
  (1, 9),
  (1, 6),
  (2, 39),
  (2, 31.8),
  (2, 23.4),
  (3, 53),
  (3, 31.8),
  (3, 21.2),
  (4, 145),
  (4, 87),
  (4, 58),
  (5, 230),
  (5, 138),
  (5, 92),
  (6, 500),
  (6, 300),
  (6, 200),
  (7, 850),
  (7, 780),
  (7, 510),
  (8, 1300),
  (8, 520),
  (8, 340);

-- Обратите внимание: убрали лишнюю единицу в начале значений и все строковые поля в одинарных кавычках.
INSERT INTO valve_components (
    valve_dn_size_id,
    cv_value_id,
    saddle_material_id,
    plunger,
    saddle,
    bushing,
    oRingSealingRing,
    stuffingBoxSeal,
    driveDiaphragm,
    setOfCovers,
    shaft,
    saddleLock
) VALUES
  (1, 1, 1, 'BM-1',  'BM-2',  'BM-3',  'BM-5',  'BM-7',  'BM-8',  'BM-9',  'BM-10', 'BM-11'),
  (2, 2, 1, 'BM-1',  'BM-12', 'BM-3',  'BM-5',  'BM-7',  'BM-8',  'BM-9',  'BM-10', 'BM-11'),
  (5, 5, 1, 'BM-13', 'BM-14', 'BM-17', 'BM-5',  'BM-7',  'BM-8',  'BM-9',  'BM-15', 'BM-16');
