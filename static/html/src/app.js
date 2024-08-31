import { FetchRecipes } from "./api/fetchData.js";
import { Dropdown } from "./models/dropdown.js";
import { Recipe } from "./models/recipe.js";
import { Search } from "./search/Search.js";

class App {

    constructor() {
        this.recipesApi = new FetchRecipes("./src/data/recipes.json")
        this.$recipesWrapper = document.querySelector('.recipes-container')
        this.search = new Search(this)

    }

    async main() {
        const recipe = await this.recipesApi.get()
        this.search.initialize(recipe)
        this.displayRecipe(recipe)


        const dropdowns = [new Dropdown({
            id: "ingredients",
            name: "Ingredients",
            class: "ddIngredients"
        }, this.search),
        new Dropdown({
            id: "appliance",
            name: "Appareils",
            class: "ddAppliance"
        }, this.search),
        new Dropdown({
            id: "ustensils",
            name: "Ustensils",
            class: "ddUstensils"
        }, this.search)]

        dropdowns.forEach(dd => this.search.subscribe(dd))

    }

    displayRecipe(recipes) {
        this.$recipesWrapper.innerHTML = ""

        if (!recipes.length) {
            return this.noRecipeView()
        }

        recipes.map(recipe => new Recipe(recipe))
            .forEach(recipe => {
                this.$recipesWrapper.appendChild(recipe.createCard())
            })
    }

    noRecipeView() {
        const div = document.createElement('div')
        div.classList.add('noRecipeView')
        const content = `<span>
        Aucune recette ne correspond à votre critère… <br/> vous pouvez
        chercher « tarte aux pommes », « poisson », etc
        </span>`
        div.innerHTML = content
        this.$recipesWrapper.appendChild(div)
    }

}

const app = new App()
app.main()