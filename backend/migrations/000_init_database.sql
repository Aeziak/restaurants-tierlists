-- Migration 000: Initialize Database
-- Description: Create database with proper encoding and timezone settings
-- Date: 2026-01-12

-- Create database (this would typically be run as superuser before connecting)
-- The database 'restaurant_tierlist' is created via docker-compose environment variables

-- Set database encoding and timezone
SET client_encoding = 'UTF8';
SET timezone = 'UTC';

-- Create extension for UUID generation (useful for future tables)
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Migration tracking table
CREATE TABLE IF NOT EXISTS schema_migrations (
    version VARCHAR(20) PRIMARY KEY,
    applied_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    description TEXT
);

-- Record this migration
INSERT INTO schema_migrations (version, description) 
VALUES ('000', 'Initialize database with UTF-8 encoding and UTC timezone')
ON CONFLICT (version) DO NOTHING;
