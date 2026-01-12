# Database Migrations

This directory contains SQL migration files for the PostgreSQL database.

## Migration Naming Convention

Migrations are numbered sequentially: `000_description.sql`, `001_description.sql`, etc.

## Applying Migrations

### With Docker Compose

When postgres container is running:

```bash
# Copy migration to container
docker cp backend/migrations/000_init_database.sql tierlist-postgres:/tmp/

# Execute migration
docker exec -it tierlist-postgres psql -U tierlist_user -d restaurant_tierlist -f /tmp/000_init_database.sql
```

### With Local PostgreSQL

```bash
psql -U tierlist_user -d restaurant_tierlist -f backend/migrations/000_init_database.sql
```

## Migration Tracking

The `schema_migrations` table tracks applied migrations:

```sql
SELECT * FROM schema_migrations ORDER BY version;
```

## Current Migrations

- **000_init_database.sql**: Initialize database with UTF-8 encoding, UTC timezone, and migration tracking table

## Future Migration Topics

Tables will be added in epic-specific stories:
- Epic 2: Authentication tables (users, sessions)
- Epic 3: Geo-navigation tables (cities, regions)
- Epic 4: Community tier list tables
- Epic 5: Restaurant search cache
- Epic 6: Personal tier list tables
