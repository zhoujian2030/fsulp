drop table if exists login_info;
create table login_info(
                    id     INTEGER PRIMARY KEY,
                    imsi   VARCHAR (16),
                    m_tmsi INTEGER,
                    time   TimeStamp NOT NULL DEFAULT (datetime('now','localtime')));