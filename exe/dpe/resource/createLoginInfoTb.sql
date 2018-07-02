DROP TABLE IF EXISTS login_info;

CREATE TABLE login_info (
    id     INTEGER      PRIMARY KEY,
    imsi   VARCHAR (16),
    m_tmsi INTEGER,
    time   TIMESTAMP    NOT NULL
                        DEFAULT (datetime('now', 'localtime') ) 
);