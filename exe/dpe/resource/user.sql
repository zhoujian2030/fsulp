DROP TABLE IF EXISTS user_login_behavior;

CREATE TABLE user_login_behavior (
    id          INTEGER      PRIMARY KEY,
    imsi        VARCHAR (16) NOT NULL,
    first_time  TIMESTAMP    NOT NULL,
    last_time   TIMESTAMP    NOT NULL,
    login_count INTEGER      NOT NULL
                             DEFAULT (1) 
);

DROP TABLE IF EXISTS user_info;

CREATE TABLE user_info (
    imsi        VARCHAR (16) PRIMARY KEY,
    login_count INTEGER      NOT NULL
                             DEFAULT (1),
    name        VARCHAR (64) 
);
