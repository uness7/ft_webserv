class FetchData {
    constructor(url) {
        this._url = url
    }

    async fetch() {
        try {
            const data = await fetch(this._url)
            const response = await data.json()
            return response
        } catch (err) {
            console.error(err)
        }
    }
}


export class FetchRecipes extends FetchData {
    constructor(url) {
        super(url)
        this._recipes = []
    }

    async get() {
        this._recipes = await this.fetch()
        return this.recipes
    }
    get recipes() {
        return this._recipes
    }

}