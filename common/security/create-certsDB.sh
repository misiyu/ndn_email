cd ~/.ndn
sqlite3 certs.db
CREATE TABLE cert (name text primary key not NULL, value text not NULL)
