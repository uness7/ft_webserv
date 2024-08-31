export class Recipe {
    constructor(data) {
        this._data = data
    }

    createCard() {
        this.$card = document.createElement('article')
        this.$card.classList.add('recipes__card')

        const content = `
        <div class="recipes__img"></div>
        <div class="recipes__details">
            <div class="recipes__details__header">
                <h1>${this.name}</h1>
                <div class="recipes__details__header-time">
                    <i class="far fa-clock"></i>
                    <p>${this.time} min</p>
                </div>
            </div>
            <div class="recipes__details__info">
                <div class="recipes__details__info-ingredient">
                    <ul> 
                        ${this.ingredientsTemplate()}
                    </ul>
                </div>
                <div class="recipes__details__info-description">
                    ${this.description}
                </div>
            </div>
        </div>
        `
        this.$card.innerHTML = content

        return this.$card
    }

    ingredientsTemplate() {
        const parser = {
            "cuillères à soupe": 'cuillères',
            "cuillères à café": "cuillères",
            'grammes': "g"
        }
        const getUnit = (unit = "") => {
            if (unit && parser[unit]) return parser[unit]
            return unit
        }

        return this.ingredients.map(el => `<li><strong>${el.ingredient}</strong>: ${el.quantity || ""} ${getUnit(el.unit || el.unite)} </li>`).join('')
    }

    get name() {
        return this._data.name
    }

    get time() {
        return this._data.time
    }

    get appliance() {
        return this._data.appliance
    }

    get ingredients() {
        return this._data.ingredients
    }

    get description() {
        return this._data.description
    }

    get ustensils() {
        return this._data.ustensils
    }
}